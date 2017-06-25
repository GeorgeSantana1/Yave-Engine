/*******************************
Copyright (c) 2016-2017 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/

#include "Mesh.h"

Result<Mesh> Mesh::from_assimp(aiMesh* mesh) {
	if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE ||
	  !mesh->HasNormals() ||
	  !mesh->HasTangentsAndBitangents() ||
	  !mesh->HasTextureCoords(0)) {
		return Err();
	}

	float radius = 0.0f;

	usize vertex_count = mesh->mNumVertices;;
	auto vertices = vector_with_capacity<Vertex>(vertex_count);
	for(usize i = 0; i != vertex_count; ++i) {
		radius = std::max(radius, mesh->mVertices[i].Length());
		vertices << Vertex {
				{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
				{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
				{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z},
				{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
			};
	}

	usize triangle_count = mesh->mNumFaces;
	auto triangles = vector_with_capacity<IndexedTriangle>(triangle_count);
	for(usize i = 0; i != triangle_count; ++i) {
		triangles << IndexedTriangle{mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]};
	}

	return Ok(Mesh{std::move(vertices), std::move(triangles), radius});
}

template<typename T>
static io::Writer::Result write_vector(io::WriterRef writer, const Vector<T>& vec) {
	auto res = writer->write_one(u32(vec.size()));
	if(res.is_error()) {
		return res;
	}
	return writer->write(vec.begin(), vec.size() * sizeof(T));
}

io::Writer::Result Mesh::write(io::WriterRef writer) const {
	u32 magic = 0x65766179;
	u32 type = 1;
	u32 version = 3;

	writer->write_one(magic);
	writer->write_one(type);
	writer->write_one(version);

	writer->write_one(radius);

	write_vector(writer, vertices);
	return write_vector(writer, triangles);
}