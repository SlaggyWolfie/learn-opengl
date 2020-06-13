#pragma once
#ifndef BASIC_MATERIAL_LIBRARY_HPP
#define BASIC_MATERIAL_LIBRARY_HPP

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <map>

class BasicMaterialLibrary
{
public:

	struct BasicMaterial
	{
		glm::vec3 ambient, diffuse, specular;
		float shine;

		BasicMaterial() : ambient(0), diffuse(0), specular(0), shine(0) {}

		BasicMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float shine)
			: ambient(ambient), diffuse(diffuse), specular(specular), shine(shine) { }
	};

	BasicMaterialLibrary()
	{
		init();
	}

	const BasicMaterial& operator[] (const std::string& material)
	{
		return _materials[material];
	}

private:
	std::map<std::string, BasicMaterial> _materials;

	void init()
	{
		// Copied from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
		// Formatted by myself.
		// Alternative sources:
		// http://devernay.free.fr/cours/opengl/materials.html
		// https://www.opengl.org/archives/resources/code/samples/sig99/advanced99/notes/node153.html
		// http://web.archive.org/web/20100725103839/http://www.cs.utk.edu/~kuck/materials_ogl.htm

		{
			//Brass
			glm::vec4 mat_ambient = { 0.329412f, 0.223529f, 0.027451f,1.0f };
			glm::vec4 mat_diffuse = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
			glm::vec4 mat_specular = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
			float shine = 27.8974f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("brass"), mat });
		}

		{
			// Bronze
			glm::vec4 mat_ambient = { 0.2125f, 0.1275f, 0.054f, 1.0f };
			glm::vec4 mat_diffuse = { 0.714f, 0.4284f, 0.18144f, 1.0f };
			glm::vec4 mat_specular = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
			float shine = 25.6f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("bronze"), mat });
		}

		{
			//Polished bronze
			glm::vec4 mat_ambient = { 0.25f, 0.148f, 0.06475f, 1.0f };
			glm::vec4 mat_diffuse = { 0.4f, 0.2368f, 0.1036f, 1.0f };
			glm::vec4 mat_specular = { 0.774597f, 0.458561f, 0.200621f, 1.0f };
			float shine = 76.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("polished bronze"), mat });
		}

		{
			//Chrome
			glm::vec4 mat_ambient = { 0.25f, 0.25f, 0.25f, 1.0f };
			glm::vec4 mat_diffuse = { 0.4f, 0.4f, 0.4f, 1.0f };
			glm::vec4 mat_specular = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
			float shine = 76.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("chrome"), mat });
		}

		{
			//Copper
			glm::vec4 mat_ambient = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
			glm::vec4 mat_diffuse = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
			glm::vec4 mat_specular = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
			float shine = 12.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("copper"), mat });
		}

		{
			//Polished copper
			glm::vec4 mat_ambient = { 0.2295f, 0.08825f, 0.0275f, 1.0f };
			glm::vec4 mat_diffuse = { 0.5508f, 0.2118f, 0.066f, 1.0f };
			glm::vec4 mat_specular = { 0.580594f, 0.223257f, 0.0695701f, 1.0f };
			float shine = 51.2f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("polished copper"), mat });
		}

		{
			//Gold
			glm::vec4 mat_ambient = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
			glm::vec4 mat_diffuse = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
			glm::vec4 mat_specular = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
			float shine = 51.2f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("gold"), mat });
		}

		{
			//Polished gold
			glm::vec4 mat_ambient = { 0.24725f, 0.2245f, 0.0645f, 1.0f };
			glm::vec4 mat_diffuse = { 0.34615f, 0.3143f, 0.0903f, 1.0f };
			glm::vec4 mat_specular = { 0.797357f, 0.723991f, 0.208006f, 1.0f };
			float shine = 83.2f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("polished gold"), mat });
		}

		{
			//Tin
			glm::vec4 mat_ambient = { 0.105882f, 0.058824f, 0.113725f, 1.0f };
			glm::vec4 mat_diffuse = { 0.427451f, 0.470588f, 0.541176f, 1.0f };
			glm::vec4 mat_specular = { 0.333333f, 0.333333f, 0.521569f, 1.0f };
			float shine = 9.84615f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("tin"), mat });
		}

		{
			//Silver
			glm::vec4 mat_ambient = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
			glm::vec4 mat_diffuse = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
			glm::vec4 mat_specular = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
			float shine = 51.2f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("silver"), mat });
		}

		{
			//Polished silver
			glm::vec4 mat_ambient = { 0.23125f, 0.23125f, 0.23125f, 1.0f };
			glm::vec4 mat_diffuse = { 0.2775f, 0.2775f, 0.2775f, 1.0f };
			glm::vec4 mat_specular = { 0.773911f, 0.773911f, 0.773911f, 1.0f };
			float shine = 89.6f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("polished silver"), mat });
		}

		{
			//Emerald
			glm::vec4 mat_ambient = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
			glm::vec4 mat_diffuse = { 0.07568f, 0.61424f, 0.07568f, 0.55f };
			glm::vec4 mat_specular = { 0.633f, 0.727811f, 0.633f, 0.55f };
			float shine = 76.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("emerald"), mat });
		}

		{
			//Jade
			glm::vec4 mat_ambient = { 0.135f, 0.2225f, 0.1575f, 0.95f };
			glm::vec4 mat_diffuse = { 0.54f, 0.89f, 0.63f, 0.95f };
			glm::vec4 mat_specular = { 0.316228f, 0.316228f, 0.316228f, 0.95f };
			float shine = 12.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("jade"), mat });
		}

		{
			//Obsidian
			glm::vec4 mat_ambient = { 0.05375f, 0.05f, 0.06625f, 0.82f };
			glm::vec4 mat_diffuse = { 0.18275f, 0.17f, 0.22525f, 0.82f };
			glm::vec4 mat_specular = { 0.332741f, 0.328634f, 0.346435f, 0.82f };
			float shine = 38.4f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("obsidian"), mat });
		}

		{
			//Perl
			glm::vec4 mat_ambient = { 0.25f, 0.20725f, 0.20725f, 0.922f };
			glm::vec4 mat_diffuse = { 1.0f, 0.829f, 0.829f, 0.922f };
			glm::vec4 mat_specular = { 0.296648f, 0.296648f, 0.296648f, 0.922f };
			float shine = 11.264f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("perl"), mat });
		}

		{
			//Ruby
			glm::vec4 mat_ambient = { 0.1745f, 0.01175f, 0.01175f, 0.55f };
			glm::vec4 mat_diffuse = { 0.61424f, 0.04136f, 0.04136f, 0.55f };
			glm::vec4 mat_specular = { 0.727811f, 0.626959f, 0.626959f, 0.55f };
			float shine = 76.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("ruby"), mat });
		}

		{
			//Turquoise
			glm::vec4 mat_ambient = { 0.1f, 0.18725f, 0.1745f, 0.8f };
			glm::vec4 mat_diffuse = { 0.396f, 0.74151f, 0.69102f, 0.8f };
			glm::vec4 mat_specular = { 0.297254f, 0.30829f, 0.306678f, 0.8f };
			float shine = 12.8f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("turquoise"), mat });
		}

		{
			//Black plastic
			glm::vec4 mat_ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
			glm::vec4 mat_diffuse = { 0.01f, 0.01f, 0.01f, 1.0f };
			glm::vec4 mat_specular = { 0.50f, 0.50f, 0.50f, 1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("black plastic"), mat });
		}

		{
			//Cyan plastic
			glm::vec4 mat_ambient = { 0.0f,0.1f,0.06f ,1.0f };
			glm::vec4 mat_diffuse = { 0.0f,0.50980392f,0.50980392f,1.0f };
			glm::vec4 mat_specular = { 0.50196078f,0.50196078f,0.50196078f,1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("cyan plastic"), mat });
		}

		{
			//Green plastic
			glm::vec4 mat_ambient = { 0.0f,0.0f,0.0f,1.0f };
			glm::vec4 mat_diffuse = { 0.1f,0.35f,0.1f,1.0f };
			glm::vec4 mat_specular = { 0.45f,0.55f,0.45f,1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("green plastic"), mat });
		}

		{
			//Red plastic
			glm::vec4 mat_ambient = { 0.0f,0.0f,0.0f,1.0f };
			glm::vec4 mat_diffuse = { 0.5f,0.0f,0.0f,1.0f };
			glm::vec4 mat_specular = { 0.7f,0.6f,0.6f,1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("red plastic"), mat });
		}

		{
			//White plastic
			glm::vec4 mat_ambient = { 0.0f,0.0f,0.0f,1.0f };
			glm::vec4 mat_diffuse = { 0.55f,0.55f,0.55f,1.0f };
			glm::vec4 mat_specular = { 0.70f,0.70f,0.70f,1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("white plastic"), mat });
		}

		{
			//Yellow plastic
			glm::vec4 mat_ambient = { 0.0f,0.0f,0.0f,1.0f };
			glm::vec4 mat_diffuse = { 0.5f,0.5f,0.0f,1.0f };
			glm::vec4 mat_specular = { 0.60f,0.60f,0.50f,1.0f };
			float shine = 32.0f;

			BasicMaterial mat(mat_ambient, mat_diffuse, mat_specular, shine);
			_materials.insert({ std::string("yellow plastic"), mat });
		}
	}
};

#endif