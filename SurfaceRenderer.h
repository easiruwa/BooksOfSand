/***********************************************************************
SurfaceRenderer - Class to render a surface defined by a regular grid in
depth image space.
Copyright (c) 2012-2016 Oliver Kreylos

This file is part of the Augmented Reality Sandbox (SARndbox).

The Augmented Reality Sandbox is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The Augmented Reality Sandbox is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Augmented Reality Sandbox; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#ifndef SURFACERENDERER_INCLUDED
#define SURFACERENDERER_INCLUDED

#include <IO/FileMonitor.h>
#include <Geometry/ProjectiveTransformation.h>
#include <GL/gl.h>
#include <GL/Extensions/GLARBShaderObjects.h>
#include <GL/GLObject.h>
#include <Kinect/FrameBuffer.h>
#include <Images/TextureSet.h> // MM: ADDED

#include "Types.h"

/* Forward declarations: */
class DepthImageRenderer;
//class ElevationColorMap; MM: commented out
class ImageMap;  // MM: added
class GLLightTracker;
class DEM;

#include "ImageMap.h"

class SurfaceRenderer:public GLObject
	{
	/* Embedded classes: */
	private:
	struct DataItem:public GLObject::DataItem
		{
		/* Elements: */
		public:
		GLuint contourLineFramebufferSize[2]; // Current width and height of contour line rendering frame buffer
		GLuint contourLineFramebufferObject; // Frame buffer object used to render topographic contour lines
		GLuint contourLineDepthBufferObject; // Depth render buffer for topographic contour line frame buffer
		GLuint contourLineColorTextureObject; // Color texture object for topographic contour line frame buffer
		unsigned int contourLineVersion; // Version number of depth image used for contour line generation
		GLhandleARB heightMapShader; // Shader program to render the surface using a height color map
		// MM: ^ this is produced by createSinglePassSurfaceShader()
		GLint heightMapShaderUniforms[16]; // Locations of the height map shader's uniform variables
		unsigned int surfaceSettingsVersion; // Version number of surface settings for which the height map shader was built
		unsigned int lightTrackerVersion; // Version number of light tracker state for which the height map shader was built
		GLhandleARB globalAmbientHeightMapShader; // Shader program to render the global ambient component of the surface using a height color map
		// MM: ^ this is produced by linkVertexAndFragmentShader (ShaderHelper.cpp)
		GLint globalAmbientHeightMapShaderUniforms[11]; // Locations of the global ambient height map shader's uniform variables
		GLhandleARB shadowedIlluminatedHeightMapShader; // Shader program to render the surface using illumination with shadows and a height color map
		// MM: ^ this is produced by linkVertexAndFragmentShader (ShaderHelper.cpp)
		GLint shadowedIlluminatedHeightMapShaderUniforms[14]; // Locations of the shadowed illuminated height map shader's uniform variables

		/* MM: not clear on the purpose of the height map shaders yet. 
		       I think they're distinct from the topography; height map
		       shaders seem to be a graphics rendering concept. */
		
		/* Constructors and destructors: */
		DataItem(void);
		virtual ~DataItem(void);
		};
	
	/* Elements: */
	//Images::TextureSet textures; // MM: ADDED - Texture set containing the image to be displayed
	const DepthImageRenderer* depthImageRenderer; // Renderer for low-level surface rendering
	unsigned int depthImageSize[2]; // Size of depth image texture
	PTransform tangentDepthProjection; // Transposed depth projection matrix for tangent planes, i.e., homogeneous normal vectors
	IO::FileMonitor fileMonitor; // Monitor to watch the renderer's external shader source files
	
	/* MM: commented out - think this is just for lining colored sections of ElevationColorMap
	bool drawContourLines; // Flag if topographic contour lines are enabled
	GLfloat contourLineFactor; // Inverse elevation distance between adjacent topographic contour lines
	*/
	
	//ElevationColorMap* elevationColorMap; // Pointer to a color map for topographic elevation map coloring MM: commented out
	ImageMap* imageMap; // MM: added
	
	DEM* dem; // Pointer to a pre-made digital elevation model to create a zero-surface for height color mapping
	GLfloat demDistScale; // Maximum deviation from surface to DEM in camera-space units
	
	bool illuminate; // Flag whether the surface shall be illuminated
		
	unsigned int surfaceSettingsVersion; // Version number of surface settings to invalidate surface rendering shader on changes
	
	/* Private methods: */
	void shaderSourceFileChanged(const IO::FileMonitor::Event& event); // Callback called when one of the external shader source files is changed
	GLhandleARB createSinglePassSurfaceShader(const GLLightTracker& lt,GLint* uniformLocations) const; // Creates a single-pass surface rendering shader based on current renderer settings
	void renderPixelCornerElevations(const int viewport[4],const PTransform& projectionModelview,GLContextData& contextData,DataItem* dataItem) const; // Creates texture containing pixel-corner elevations based on the current depth image
	
	/* Constructors and destructors: */
	public:
	SurfaceRenderer(const DepthImageRenderer* sDepthImageRenderer); // Creates a renderer for the given depth image renderer
	
	/* Methods from GLObject: */
	virtual void initContext(GLContextData& contextData) const;
	
	/* New methods: */
	/* MM: commented out - think this is just for lining colored sections of ElevationColorMap
	void setDrawContourLines(bool newDrawContourLines); // Enables or disables topographic contour lines
	void setContourLineDistance(GLfloat newContourLineDistance); // Sets the elevation distance between adjacent topographic contour lines
	*/
	//void setElevationColorMap(ElevationColorMap* newElevationColorMap); // Sets an elevation color map MM: commented out
	void setImageMap(ImageMap* newImageMap);
	void setDem(DEM* newDem); // Sets a pre-made digital elevation model to create a zero surface for height color mapping
	void setDemDistScale(GLfloat newDemDistScale); // Sets the deviation from DEM to surface to saturate the deviation color map
	void setIlluminate(bool newIlluminate); // Sets the illumination flag
	void renderSinglePass(const int viewport[4],const PTransform& projection,const OGTransform& modelview,GLContextData& contextData) const; // Renders the surface in a single pass using the current surface settings
	// MM: PTransform is defined in Types.h as Geometry::ProjectiveTransformation<Scalar,3>
	//     It's a type for 3D projective transformations (4x4 matrices)
	};

#endif
