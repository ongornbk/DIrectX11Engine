#include "ResourceTexture.h"



ResourceTexture::ResourceTexture(void)
{
	m_texture = NULL;
}


ResourceTexture::~ResourceTexture(void)
{
	if (m_texture)
	{
		delete m_texture;
		m_texture = NULL;
	}
}

bool ResourceTexture::Load(ID3D11Device * device, WCHAR* textureFileName)
{
	m_texture = new class Texture();
	if (!m_texture->Initialize(device, textureFileName))
	{
		delete m_texture;
		m_texture = NULL;
		return false;
	}
	return true;
}

Texture * ResourceTexture::GetTexture()
{
	return m_texture;
}

string ResourceTexture::GetName()
{
	if (m_texture)
	{
		return m_texture->GetName();
	}
	return "Uninitialized Texture";
}
