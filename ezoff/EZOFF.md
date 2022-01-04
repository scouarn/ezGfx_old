The EZOFF format
================


## Purpose

* Lack of a format that provides all the mesh data I need
* Easy to parse
* Face color
* Vertex color
* UVs
* Reference to texture file


## Sections 

* Header
* Vertices
* UVs
* Material section
* Faces


## Header section 

* v_count n => number of vertices
* vt_count n => number of uv coordinates
* f_count n => number of faces
* m_count n => number of materials


## Vertex section

* Made of v_count lines (no empty lines)
*     pos     color   uv    normal
* v   x y z   r g b   u v   nx ny nz


## Material section

* Made of m_count lines (no empty lines)
*     texture (or "null")  color
* m   texture.bmp          r g b

## Face section

* Make of f_count lines (no empty lines)
*     vertices   material
* f   {p1 u v} {p2 u v} {p3 u v} mat
