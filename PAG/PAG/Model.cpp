#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Logger.h"

void Model::draw(Shader* shader, const Transform wvp, const Transform model, const bool gui)
{
	setWorld(model.translate(pos).rotate(rot).scale(scale));
	/* Get uniform location and send MVP matrix there */
	shader->setMat4("model", world.getMatrix());
	shader->setMat4("wvp", wvp.getMatrix());
	shader->setMat3("normalMat", normalMat);
	for (auto &m : meshes)
		m.draw(shader);
	for (auto &mdl : children)
		mdl.draw(shader, wvp, world, gui);
}

void Model::drawColor(Shader* shader, const Transform wvp) {
	shader->setMat4("model", world.getMatrix());
	shader->setMat4("wvp", wvp.getMatrix());
	for (auto &m : meshes)
		m.drawColor(shader, id);
	for (auto &mdl : children)
		mdl.drawColor(shader, wvp);
}

vector<Mesh>& Model::getMeshes()
{
	return meshes;
}

vector<Model>& Model::getChildren()
{
	return children;
}

void Model::loadModel(string path)
{
	Assimp::Importer import;
	const auto scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::logError(string_format("ASSIMP::%s", import.GetErrorString()));
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	aiVector3t<float> ai_scale;
	aiQuaterniont<float> ai_quaterniont;
	aiVector3t<float> ai_pos;
	node->mTransformation.Decompose(ai_scale, ai_quaterniont, ai_pos);
	scale.x = ai_scale.x;
	scale.y = ai_scale.y;
	scale.z = ai_scale.z;
	rot.x = ai_quaterniont.x;
	rot.y = ai_quaterniont.y;
	rot.z = ai_quaterniont.z;
	pos.x = ai_pos.x;
	pos.y = ai_pos.y;
	pos.z = ai_pos.z;
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		children.push_back(Model(node->mChildren[i], scene, directory));
		//processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	//vector<Texture> textures;
	Material material;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		if (mesh->mVertices[i].x > aabb_max.x)
			aabb_max.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y > aabb_max.y)
			aabb_max.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z > aabb_max.z)
			aabb_max.z = mesh->mVertices[i].z;
		if (mesh->mVertices[i].x < aabb_min.x)
			aabb_min.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y < aabb_min.y)
			aabb_min.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z < aabb_min.z)
			aabb_min.z = mesh->mVertices[i].z;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vec;
		}
		else
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		const auto ai_material = scene->mMaterials[mesh->mMaterialIndex];
		
		auto diffuse_maps = loadMaterialTextures(ai_material,
			aiTextureType_DIFFUSE, "texture_diffuse", scene);
		material.textures.insert(material.textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		auto specular_maps = loadMaterialTextures(ai_material,
			aiTextureType_SPECULAR, "texture_specular", scene);
		material.textures.insert(material.textures.end(), specular_maps.begin(), specular_maps.end());
		ai_material->Get(AI_MATKEY_SHININESS, material.shininess);
	}

	return Mesh(vertices, indices, material);
}

void Model::updateNormalMatrix()
{
	normalMat = glm::transpose(glm::inverse(world.getMatrix()));
}

void Model::setWorld(const Transform& world)
{
	this->world = world;
	updateNormalMatrix();
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, const aiTextureType type, const string typeName, const aiScene *scene)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		auto skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			// check if texture is embedded
			if(str.data[0] == '*')
			{
				const size_t idx = atoi(str.C_Str()+1);
				if(texture.loadTexture(scene->mTextures[idx]))
				{
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture); // add to loaded 
				}
			}
			else if (texture.loadTexture(str.C_Str(), directory))
			{
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // add to loaded textures
			}
		}
	}
	return textures;
}
