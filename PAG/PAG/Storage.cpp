#include "Storage.h"
#include "Logger.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <sstream>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

// Loads content from level.map file, shader is used for lighting
bool Storage::loadContent(Shader *shader, GraphNode* root)
{
	std::ifstream content_manifest_file("level.map");
	std::string line;
	auto count = 0;
	if (!content_manifest_file)
	{
		Logger::logError("Could not open file level.map!");
		content_manifest_file.close();
		return false;
	}
	//Assert version
	do
	{
		getline(content_manifest_file, line);
		count++;
	} while (line.find('#') != std::string::npos);
	if (line != "PAG6_3")
	{
		Logger::logError(string_format("File level.map is not supported! Line:%d", count));
		content_manifest_file.close();
		return false;
	}
	while (content_manifest_file.good())
	{
		getline(content_manifest_file, line);
		count++;
		if (line[0] == '#') continue;
		auto x = split(line, ':');
		if (x[0] == "MDL")
		{
			// Assert size 6
			if (x.size() != 6)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			glm::vec3 pos, rot, scale;
			auto pos_str = split(x[2], ',');
			if (pos_str.size() == 0)
				pos = glm::vec3(0.0f);
			else if (pos_str.size() == 1)
				pos = glm::vec3(atof(pos_str[0].c_str()));
			else if (pos_str.size() == 3)
				pos = glm::vec3(atof(pos_str[0].c_str()), atof(pos_str[1].c_str()), atof(pos_str[2].c_str()));
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			auto rot_str = split(x[3], ',');
			if (rot_str.size() == 0)
				rot = glm::vec3(0.0f);
			else if (rot_str.size() == 1)
				rot = glm::vec3(glm::radians(atof(rot_str[0].c_str())));
			else if (rot_str.size() == 3)
			{
				float f1 = atof(rot_str[0].c_str());
				float f2 = atof(rot_str[1].c_str());
				float f3 = atof(rot_str[2].c_str());
				rot = glm::vec3(glm::radians(f1), glm::radians(f2), glm::radians(f3));
			}
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			auto scale_str = split(x[4], ',');
			if (scale_str.size() == 0)
				scale = glm::vec3(1.0f);
			else if (scale_str.size() == 1)
				scale = glm::vec3(atof(scale_str[0].c_str()));
			else if (scale_str.size() == 3)
			{
				float f1 = atof(scale_str[0].c_str());
				float f2 = atof(scale_str[1].c_str());
				float f3 = atof(scale_str[2].c_str());
				scale = glm::vec3(f1, f2, f3);
			}
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			auto refractive = false;
			std::istringstream refr_bool(x[5]);
			if (!(refr_bool>>refractive))
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			Model* mdl = new Model(x[1], refractive);
			models.addModel(mdl);
			nodes.addNode(new GraphNode(mdl));
			nodes.getAllNodes().back()->setTransform(nodes.getAllNodes().back()->getTransform().translate(pos).rotate(rot).scale(scale));
			root->addChild(nodes.getAllNodes().back());
		}
		else if (x[0] == "DIL")
		{
			// Assert size 5
			if (x.size() != 5)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			glm::vec3 val[4];
			for (auto i = 0; i < 4; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return false;
				}
			}
			lights.addLight(new DirectionalLight(val[0], val[1], val[2], val[3]));
			nodes.addNode(new GraphNode(lights.getAllLights().back()));
			root->addChild(nodes.getAllNodes().back());
			lights.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "POL")
		{
			// Assert size 8
			if (x.size() != 8)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			glm::vec3 val[4];
			for (auto i = 0; i < 4; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return false;
				}
			}
			float vf[3];
			for (auto i = 0; i < 3; i++)
			{
				auto str = split(x[i + 5], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						vf[i] = 0.0f;
					else
					{
						vf[i] = atof(str[0].c_str());
					}
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return false;
				}
			}
			lights.addLight(new PointLight(val[0], val[1], val[2], val[3], vf[0], vf[1], vf[2]));
			nodes.addNode(new GraphNode(lights.getAllLights().back()));
			root->addChild(nodes.getAllNodes().back());
			lights.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "SPL")
		{
			// Assert size 11
			if (x.size() != 11)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			glm::vec3 val[5];
			for (auto i = 0; i < 5; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError("File level.map is not supported!");
					content_manifest_file.close();
					return false;
				}
			}
			float vf[5];
			for (auto i = 0; i < 5; i++)
			{
				auto str = split(x[i + 6], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						vf[i] = 0.0f;
					else
					{
						vf[i] = atof(str[0].c_str());
					}
				}
				else
				{
					Logger::logError("File level.map is not supported!");
					content_manifest_file.close();
					return false;
				}
			}
			lights.addLight(new SpotLight(val[0], val[1], val[2], val[3], val[4], glm::cos(glm::radians(vf[0])), glm::cos(glm::radians(vf[1])), vf[2], vf[3], vf[4]));
			nodes.addNode(new GraphNode(lights.getAllLights().back()));
			root->addChild(nodes.getAllNodes().back());
			lights.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "SKY")
		{
			// Assert size 7
			if (x.size() != 7)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}

			vector<std::string> faces;
			for (auto i = 1; i < 7; i++)
			{
				faces.push_back(x[i]);
			}
			skybox.loadCubemap(faces);
		}
		else if (x[0] == "PAR")
		{
			// Assert size 3
			if (x.size() != 3)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			if(!particle_texture.loadTexture(x[1].c_str(), "Assets"))
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			glm::vec3 pos;
			auto str = split(x[2], ',');
			if (str.size() == 1)
			{
				if (str[0].empty())
					pos = glm::vec3(0.0f);
				else
				{
					pos = glm::vec3(atof(str[0].c_str()));
				}
			}
			else if (str.size() == 3)
			{
				pos = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
			}
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return false;
			}
			particle_node = new GraphNode(nullptr);
			particle_node->setTransform(particle_node->getTransform().translate(pos));
			nodes.addNode(particle_node);
			root->addChild(particle_node);
		}
		else
		{
			Logger::logError(string_format("File level.map is not supported! Line:%d", count));
			content_manifest_file.close();
			return false;
		}
	}
	content_manifest_file.close();
	return true;
}

void Storage::clear()
{
	nodes.clear();
	models.clear();
}