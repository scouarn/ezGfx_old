#include "ezSfx_core.h"

#include <stdio.h>
#include <stdlib.h>



#ifdef EZSFX_CORE


//http://soundfile.sapp.org/doc/WaveFormat/
//https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f

//Wave file header

struct riff_header {

		//RIFF chunk descriptor
		int8_t  ChunkID[4];  //"RIFF" -> 0x52494646
		int32_t ChunkSize;   //Size of the rest of the file
		int8_t  Format[4];   //"WAVE" -> 0x57415645
};

struct fmt_subchunk {

		int8_t  SubchunkID[4]; // "fmt " -> 0x666d7420
		int32_t SubchunkSize;  // Should be 16 for PCM
		int16_t AudioFormat;   // 1 -> PCM
		int16_t NumChannels;
		int32_t SampleRate;
		int32_t ByteRate;      // SampleRate * NumChannels * sizeof(sample)
		int16_t BlockAlign;    // NumChannels * sizeof(sample) -> bytes per alsa "frame"
		int16_t BitsPerSample; // 8 * sizeof(sample)
};

struct data_subchunk {

		int8_t  SubchunkID[4]; // "data" -> 0x64617461
		int32_t SubchunkSize;  //Number of bytes in data. 
		// -> samples * num_channels * sizeof(sample)
		//uint8_t data[SubchunkSize];
};




EZ_PCMArray EZ_load_WAV(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_PCMArray arr;

	if (file == NULL) {
		WARNING("Couldn't open file %s\n", fname);
		return arr;
	}


	//read riff header
	struct riff_header riff;
	fread(&riff, sizeof(struct riff_header), 1, file);

	if (*(uint32_t*)riff.ChunkID != 0x46464952) {
		WARNING("Wrong magic number ('RIFF') in %s",fname);
		fclose(file);
		return arr;
	}

	if (*(uint32_t*)riff.Format != 0x45564157) {
		WARNING("Wrong magic number ('WAVE') in %s",fname);
		fclose(file);
		return arr;
	}


	//read ftm chunk
	struct fmt_subchunk fmt;
	fread(&fmt, sizeof(struct fmt_subchunk), 1, file);

	if (*(uint32_t*)fmt.SubchunkID != 0x20746d66) {
		WARNING("Wrong magic number ('fmt ') in %s",fname);
		fclose(file);
		return arr;
	}

	if (fmt.SubchunkSize != 16 || fmt.AudioFormat != 1) {
		WARNING("Only PCM wave file are supported, %s", fname);
		fclose(file);
		return arr;
	}

	if (fmt.NumChannels == 0) {
		WARNING("This audio file has 0 channels %s", fname);
		fclose(file);
		return arr;
	}
	else arr.channels = fmt.NumChannels;

	if (fmt.SampleRate == 0 || fmt.ByteRate == 0) {
		WARNING("This audio file has a sample rate of 0... %s", fname);
		fclose(file);
		return arr;
	}
	else arr.sampleRate = fmt.SampleRate;

	if (fmt.BitsPerSample != 16) {
		WARNING("Only 16bits signed integer audio is supported, %s", fname);
		fclose(file);
		return arr;
	}


	//read data chunk
	struct data_subchunk data;
	fread(&data, sizeof(struct data_subchunk), 1, file);

	if (*(uint32_t*)data.SubchunkID != 0x61746164) {
		WARNING("Wrong magic number ('data') in %s",fname);
		fclose(file);
		return arr;
	}

	if (data.SubchunkSize == 0) {
		WARNING("This audio file has no samples, %s", fname);
		fclose(file);
		return arr;
	}
	else arr.size = data.SubchunkSize / fmt.NumChannels;


	//read samples
	arr.data = malloc(data.SubchunkSize);

	if (arr.data == NULL) {
		WARNING("Couldn't allocate memory for file, %s", fname);
		fclose(file);
		return arr;
	}
	else arr.curr = arr.data;
	
	fread(arr.data, 1, data.SubchunkSize, file);


	return arr;

}


void EZ_save_WAV(const EZ_PCMArray arr, const char* fname) {

	//open file
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	//write headers
	struct riff_header riff = {
		.ChunkID = "RIFF",
		.ChunkSize = 36 + arr.size * sizeof(EZ_Sample),
		.Format  = "WAVE"
	};

	fwrite(&riff, sizeof(struct riff_header), 1, file);


	struct fmt_subchunk fmt = {
		.SubchunkID = "fmt ",
		.SubchunkSize = 16,
		.AudioFormat = 1,
		.NumChannels = arr.channels,
		.SampleRate  = arr.sampleRate,
		.ByteRate = arr.channels * arr.sampleRate * sizeof(EZ_Sample),
		.BlockAlign = arr.channels * sizeof(EZ_Sample),
		.BitsPerSample = 8 * sizeof(EZ_Sample)
	};

	fwrite(&fmt, sizeof(struct fmt_subchunk), 1, file);


	struct data_subchunk data = {
		.SubchunkID = "data",
		.SubchunkSize = arr.size * sizeof(EZ_Sample)
	};

	fwrite(&data, sizeof(struct data_subchunk), 1, file);


	//write pcm data
	fwrite(arr.data, sizeof(EZ_Sample), arr.size, file);

}




EZ_PCMArray EZ_load_PCM(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_PCMArray arr;

	if (file == NULL) {
		WARNING("Couldn't open file %s\n", fname);
		return arr;
	}

	//get the size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (fileSize == 0) {
		WARNING("File %s was of size 0", fname);
		return arr;
	}


	//init data
	arr.data = malloc(fileSize);

	if (arr.data == NULL) {
		WARNING("Couldn't allocate memory for PCM %s", fname);
		fclose(file);
		return arr;
	}

	//set array parameters
	arr.curr = arr.data;
	arr.size = fileSize / sizeof(EZ_Sample);
	arr.sampleRate = CD44KHZ; //CANT TELL
	arr.channels = 2; //CANT TELL

	//read samples
	fread(arr.data, sizeof(EZ_Sample), arr.size, file);

	//return
	fclose(file);
	return arr;
}


void EZ_save_PCM(const EZ_PCMArray arr, const char* fname) {}

#endif