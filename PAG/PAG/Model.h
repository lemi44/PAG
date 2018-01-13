#pragma once
#include <string>
#include "Texture.h"
#include "Shader.h"
#include <vector>
#include "Mesh.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "Transform.h"
#include "Drawable.h"

using namespace std;

class Model : public Drawable
{
public:
	/*  Functions   */
	Model(const string path, bool gamma = false) : Drawable(), aabb_min(0.f), aabb_max(0.f), pos(0.f), rot(0.f), scale(1.0f), gammaCorrection(gamma)
	{
		loadModel(path);
		world = Transform::origin();
	}
	Model(aiNode *node, const aiScene *scene, const string directory, bool gamma = false) : Drawable(), aabb_min(0.f), aabb_max(0.f), pos(0.f), rot(0.f), scale(1.0f), gammaCorrection(gamma)
	{
		this->directory = directory;
		world = Transform::origin();
		processNode(node, scene);
	}
	void draw(Shader* shader, const Transform wvp, const Transform model, const bool gui) final;
	void drawColor(Shader * shader, const Transform wvp) final;
	virtual vector<Mesh>& getMeshes() override;
	vector<Model>& getChildren();
	glm::vec3 aabb_min;
	glm::vec3 aabb_max;
	Transform world;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
	glm::mat3 normalMat;
	bool gammaCorrection;
private:
	/*  Model Data  */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;
	vector<Model> children;
	/*  Functions   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	void updateNormalMatrix();
	void setWorld(const Transform& world);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName, const aiScene *scene);
};
