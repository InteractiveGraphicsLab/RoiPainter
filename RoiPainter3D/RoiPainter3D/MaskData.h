#ifndef MASKDATA_H_
#define MASKDATA_H_
#pragma unmanaged

#include <string>
#include "tmath.h"
#include "tmesh.h"

class MaskData
{
public:
	std::string m_name;
	TMesh  m_surface;
	EVec3i m_color;
	double m_alpha;
	bool   m_b_drawsurface;
	bool   m_b_locked;

	MaskData(std::string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false) 
	{
		m_name      = _name;
		m_color     = _color;
		m_alpha     = _alpha;
		m_b_drawsurface = _bRendSurf;
		m_b_locked   = _lock;
	}

	void Set(const MaskData& v)
	{
		m_name    = v.m_name;
		m_surface = v.m_surface;
		m_color   = v.m_color;
		m_alpha   = v.m_alpha;
		m_b_drawsurface = v.m_b_drawsurface;
		m_b_locked      = v.m_b_locked;
	}

	MaskData( const MaskData& v)
	{
		Set(v);
	}
	
	MaskData& operator=(const MaskData& v)
	{
		Set(v);
		return *this;
	}
};


#endif
