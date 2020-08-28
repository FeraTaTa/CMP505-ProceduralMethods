#include  "stdafx.h"
#include  "textureclass.h"
#include  "terrainclass.h"
#include  < stdio.h >
 
 
TerrainClass:: TerrainClass()
{
}
 
 
TerrainClass:: TerrainClass( const  TerrainClass &  other)
{
}
 
 
TerrainClass::~ TerrainClass()
{
}
 
 
bool  TerrainClass::Initialize(ID3D11Device *  device,  const  char *  heightMapFilename,  const  WCHAR *  textureFilename)
{
	// Load in the height map for the terrain.
    if ( ! LoadHeightMap(heightMapFilename))
    {
        return  false ;
    }
 
    // Normalize the height of the height map.
    NormalizeHeightMap();
 
    // Calculate the normal of the terrain data.
    if ( ! CalculateNormals())
    {
        return  false ;
    }
 
    // Compute the texture coordinates.
    CalculateTextureCoordinates();
 
    // Load the texture.
    if ( ! LoadTexture(device, textureFilename))
    {
        return  false ;
    }
 
	// Initialize the vertex and index buffer that hold the geometry for the terrain.
    return  InitializeBuffers(device);
}
 
 
void  TerrainClass::Shutdown()
{
    // Release the texture.
    ReleaseTexture();
 
	// Release the vertex and index buffer.
    ShutdownBuffers();
 
    // Release the height map data.
    ShutdownHeightMap();
}
 
 
ID3D11ShaderResourceView *  TerrainClass::GetTexture()
{
    return  m_Texture -> GetTexture();
}
 
 
bool  TerrainClass::LoadHeightMap( const  char *  filename)
{
	// Open the height map file in binary.
    FILE *  filePtr  =  nullptr;
    if (fopen_s( & filePtr, filename,  "rb" )  !=  0 )
    {
        return  false ;
    }
 
	// Read in the file header.
    BITMAPFILEHEADER bitmapFileHeader;
    if (fread( & bitmapFileHeader,  sizeof (BITMAPFILEHEADER),  1 , filePtr)  !=  1 )
    {
        return  false ;
    }
 
	// Read in the bitmap info header.
    BITMAPINFOHEADER bitmapInfoHeader;
    if (fread( & bitmapInfoHeader,  sizeof (BITMAPINFOHEADER),  1 , filePtr)  !=  1 )
    {
        return  false ;
    }
 
	// Save the dimensions of the terrain.
    m_terrainWidth  =  bitmapInfoHeader.biWidth;
    m_terrainHeight  =  bitmapInfoHeader.biHeight;
 
    // Calculate the size of the bitmap image data.
    int  imageSize  =  m_terrainWidth  *  m_terrainHeight  *  3 ;
 
	// Allocate memory for the bitmap image data.
    unsigned  char *  bitmapImage  =  new  unsigned  char [imageSize];
    if ( ! bitmapImage)
    {
        return  false ;
    }
 
    // Move to the beginning of the bitmap data.
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
 
	// Read in the bitmap image data.
    if (fread(bitmapImage,  1 , imageSize, filePtr)  !=  imageSize)
    {
        return  false ;
    }
 
    // Close the file.
    if (fclose(filePtr)  !=  0 )
    {
        return  false ;
    }
 
	// Create the structure to hold the height map data.
    m_heightMap  =  new  HeightMapType[m_terrainWidth  *  m_terrainHeight];
    if ( ! m_heightMap)
    {
        return  false ;
    }
 
	// Initialize the position in the image data buffer.
    int  k  =  0 ;
 
    // Read the image data into the height map.
    for ( int  j = 0 ; j < m_terrainHeight; j ++ )
    {
        for ( int  i = 0 ; i < m_terrainWidth; i ++ )
        {
            unsigned  char  height  =  bitmapImage[k];
            
            int  index  =  (m_terrainHeight  *  j)  +  i;
 
            m_heightMap[index].x  =  ( float )i;
            m_heightMap[index].y  =  ( float )height;
            m_heightMap[index].z  =  ( float )j;
 
            k += 3 ;
        }
    }
 
    // Release the bitmap image data.
    delete  [] bitmapImage;
    bitmapImage  =  0 ;
 
    return  true ;
}
 
 
void  TerrainClass::NormalizeHeightMap()
{
    for ( int  j = 0 ; j < m_terrainHeight; j ++ )
    {
        for ( int  i = 0 ; i < m_terrainWidth; i ++ )
        {
            m_heightMap[(m_terrainHeight  *  j)  +  i].y  /=  15.0f;
        }
    }
}
 
 
bool  TerrainClass::CalculateNormals()
{
    int  index1  =  0 ;
    int  index2  =  0 ;
    int  index3  =  0 ;
    int  index  =  0 ;
    int  count  =  0 ;
    float  vertex1[ 3 ]  =  {  0.f,  0.f,  0.f };
    float  vertex2[ 3 ]  =  {  0.f,  0.f,  0.f };
    float  vertex3[ 3 ]  =  {  0.f,  0.f,  0.f };
    float  vector1[ 3 ]  =  {  0.f,  0.f,  0.f };
    float  vector2[ 3 ]  =  {  0.f,  0.f,  0.f };
    float  sum[ 3 ]  =  {  0.f,  0.f,  0.f };
	double length = 0.0f;
 
 
    // Create a temporary array to hold the unnormalized normal vector.
    VectorType *  normals  =  new  VectorType[(m_terrainHeight - 1 )  *  (m_terrainWidth - 1 )];
    if ( ! normals)
    {
        return  false ;
    }
 
    // Go through all sides of the mesh and calculate the normal.
    for ( int  j = 0 ; j < (m_terrainHeight - 1 ); j ++ )
    {
        for ( int  i = 0 ; i < (m_terrainWidth - 1 ); i ++ )
        {
            index1  =  (j  *  m_terrainHeight)  +  i;
            index2  =  (j  *  m_terrainHeight)  +  (i + 1 );
            index3  =  ((j + 1 )  *  m_terrainHeight)  +  i;
 
            // Get the three vertices from the surface.
            vertex1[ 0 ]  =  m_heightMap[index1].x;
            vertex1[ 1 ]  =  m_heightMap[index1].y;
            vertex1[ 2 ]  =  m_heightMap[index1].z;
        
            vertex2[ 0 ]  =  m_heightMap[index2].x;
            vertex2[ 1 ]  =  m_heightMap[index2].y;
            vertex2[ 2 ]  =  m_heightMap[index2].z;
        
            vertex3[ 0 ]  =  m_heightMap[index3].x;
            vertex3[ 1 ]  =  m_heightMap[index3].y;
            vertex3[ 2 ]  =  m_heightMap[index3].z;
 
            // Compute the two vectors of the surface.
            vector1 [ 0 ]  =  vertex1 [ 0 ]  -  vertex3 [ 0 ];
            vector1 [ 1 ]  =  vertex1 [ 1 ]  -  vertex3 [ 1 ];
            vector1 [ 2 ]  =  vertex1 [ 2 ]  -  vertex3 [ 2 ];
            vector2 [ 0 ]  =  vertex3 [ 0 ]  -  vertex2 [ 0 ];
            vector2 [ 1 ]  =  vertex3 [ 1 ]  -  vertex2 [ 1 ];
            vector2 [ 2 ]  =  vertex3 [ 2 ]  -  vertex2 [ 2 ];
 
            index  =  (j  *  (m_terrainHeight - 1 ))  +  i;
 
            // Compute the cross product of the two vectors to get the unnormalized values ​​for these two normals.
            normals[index].x  =  (vector1[ 1 ]  *  vector2[ 2 ])  -  (vector1[ 2 ]  *  vector2[ 1 ]);
            normals[index].y  =  (vector1[ 2 ]  *  vector2[ 0 ])  -  (vector1[ 0 ]  *  vector2[ 2 ]);
            normals[index].z  =  (vector1[ 0 ]  *  vector2[ 1 ])  -  (vector1[ 1 ]  *  vector2[ 0 ]);
        }
    }
 
    // Now go through all the vertices and take the average of each side.     
    // A vertex touches and gets the average average value for that vertex.
    for ( int  j = 0 ; j < m_terrainHeight; j ++ )
    {
        for ( int  i = 0 ; i < m_terrainWidth; i ++ )
        {
            // Initialize the sum.
            sum[ 0 ]  =  0.0f;
            sum[ 1 ]  =  0.0f;
            sum[ 2 ]  =  0.0f;
 
            // Initialize the count.
            count  =  0 ;
 
            // Bottom left side.
            if (((i - 1 )  >=  0 )  &&  ((j - 1 )  >=  0 ))
            {
                index  =  ((j - 1 )  *  (m_terrainHeight - 1 ))  +  (i - 1 );
 
                sum[ 0 ]  +=  normals[index].x;
                sum[ 1 ]  +=  normals[index].y;
                sum[ 2 ]  +=  normals[index].z;
                count ++ ;
            }
 
            // Bottom right side.
            if ((i  <  (m_terrainWidth - 1 ))  &&  ((j - 1 )  >=  0 ))
            {
                index  =  ((j - 1 )  *  (m_terrainHeight - 1 ))  +  i;
 
                sum[ 0 ]  +=  normals[index].x;
                sum[ 1 ]  +=  normals[index].y;
                sum[ 2 ]  +=  normals[index].z;
                count ++ ;
            }
 
            // Top left side.
            if (((i - 1 )  >=  0 )  &&  (j  <  (m_terrainHeight - 1 )))
            {
                index  =  (j  *  (m_terrainHeight - 1 ))  +  (i - 1 );
 
                sum[ 0 ]  +=  normals[index].x;
                sum[ 1 ]  +=  normals[index].y;
                sum[ 2 ]  +=  normals[index].z;
                count ++ ;
            }
 
            // Top right side.
            if ((i  <  (m_terrainWidth - 1 ))  &&  (j  <  (m_terrainHeight - 1 )))
            {
                index  =  (j  *  (m_terrainHeight - 1 ))  +  i;
 
                sum[ 0 ]  +=  normals[index].x;
                sum[ 1 ]  +=  normals[index].y;
                sum[ 2 ]  +=  normals[index].z;
                count ++ ;
            }
            
            // Take the average of the faces touching this vertex.
            sum[ 0 ]  =  (sum[ 0 ]  /  ( float )count);
            sum[ 1 ]  =  (sum[ 1 ]  /  ( float )count);
            sum[ 2 ]  =  (sum[ 2 ]  /  ( float )count);
 
            // Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
            
            // Get the index of the vertex position in the height map array.
            index  =  (j  *  m_terrainHeight)  +  i;
 
            // Normalize the final shared normal of this vertex and store it in the height map array.
            m_heightMap[index].nx  =  (sum[ 0 ]  /  length);
            m_heightMap[index].ny  =  (sum[ 1 ]  /  length);
            m_heightMap[index].nz  =  (sum[ 2 ]  /  length);
        }
    }
 
    // Release the temporary normal.
    delete  [] normals;
    normals  =  0 ;
 
    return  true ;
}
 
 
void  TerrainClass::ShutdownHeightMap()
{
    if (m_heightMap)
    {
        delete  [] m_heightMap;
        m_heightMap  =  0 ;
    }
}
 
 
void  TerrainClass::CalculateTextureCoordinates()
{
    // Calculate how much to increment the texture coordinates.
    float  incrementValue  =  ( float )TEXTURE_REPEAT  /  ( float )m_terrainWidth;
 
    // Count the number of times to repeat the texture.
    int  incrementCount  =  m_terrainWidth  /  TEXTURE_REPEAT;
 
    // Initialize the tu and tv coordinate values.
    float  tuCoordinate  =  0.0f;
    float  tvCoordinate  =  1.0f;
 
    // Initialize the tu and tv coordinate indices.
    int  tuCount  =  0 ;
    int  tvCount  =  0 ;
 
    // Iterate over the full height map and compute the tu and tv texture coordinates for each vertex.
    for ( int  j = 0 ; j < m_terrainHeight; j ++ )
    {
        for ( int  i = 0 ; i < m_terrainWidth; i ++ )
        {
            // Store the texture coordinates in the height map.
            m_heightMap[(m_terrainHeight  *  j)  +  i].tu  =  tuCoordinate;
            m_heightMap[(m_terrainHeight  *  j)  +  i].tv  =  tvCoordinate;
 
            // Increase the tu texture coordinate by the increment value and increase the index by 1.
            tuCoordinate  +=  incrementValue;
            tuCount ++ ;
 
            // Make sure you are at the right end of the texture, and if so, start over.
            if (tuCount  ==  incrementCount)
            {
                tuCoordinate  =  0.0f;
                tuCount  =  0 ;
            }
        }
 
        // Increment the tv texture coordinate by the increment value and increment the index by 1.
        tvCoordinate  -=  incrementValue;
        tvCount ++ ;
 
        // Make sure it is at the top of the texture, and if so, start over at the bottom.
        if (tvCount  ==  incrementCount)
        {
            tvCoordinate  =  1.0f;
            tvCount  =  0 ;
        }
    }
}
 
 
bool  TerrainClass::LoadTexture(ID3D11Device *  device,  const  WCHAR *  filename)
{
    // Create the texture object.
    m_Texture  =  new  TextureClass;
    if ( ! m_Texture)
    {
        return  false ;
    }
 
    // Initialize the texture object.
    return  m_Texture -> Initialize (device, filename );
}
 
 
void  TerrainClass::ReleaseTexture()
{
    // Release the texture object.
    if (m_Texture)
    {
        m_Texture -> Shutdown();
        delete  m_Texture;
        m_Texture  =  0 ;
    }
}
 
 
bool  TerrainClass::InitializeBuffers(ID3D11Device *  device)
{
    float  tu  =  0.0f;
    float  tv  =  0.0f;
 
    // Calculate the number of vertices in the terrain mesh.
    m_vertexCount  =  (m_terrainWidth  -  1 )  *  (m_terrainHeight  -  1 )  *  6 ;
 
    // Create the vertex array.
    m_vertices  =  new  VertexType[m_vertexCount];
    if ( ! m_vertices)
    {
        return  false ;
    }
 
	// Initialize the index to the vertex buffer.
    int  index  =  0 ;
 
	// Load the vertex and index array with the terrain data.
    for ( int  j = 0 ; j < (m_terrainHeight - 1 ); j ++ )
    {
        for ( int  i = 0 ; i < (m_terrainWidth - 1 ); i ++ )
        {
			int index1 = (m_terrainHeight * j) + i;          // Bottom left.
			int index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			int index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.
 
			// Upper left.
            tv  =  m_heightMap[index3].tv;
 
            // Modify the texture coordinates to cover the top edge.
            if (tv  ==  1.0f) {tv  =  0.0f; }
 
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
            m_vertices[index].texture  =  XMFLOAT2(m_heightMap[index3].tu, tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
            index ++ ;
 
			// Upper right.
            tu  =  m_heightMap[index4].tu;
            tv  =  m_heightMap[index4].tv;
 
            // Modify the texture coordinates to cover the top and right edges.
            if (tu  ==  0.0f) {tu  =  1.0f; }
            if (tv  ==  1.0f) {tv  =  0.0f; }
 
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            m_vertices[index].texture  =  XMFLOAT2(tu, tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
            index ++ ;
 
			// Bottom left.
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            m_vertices[index].texture  =  XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
            index ++ ;
 
			// Bottom left.
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            m_vertices[index].texture  =  XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
            index ++ ;
 
			// Upper right.
            tu  =  m_heightMap[index4].tu;
            tv  =  m_heightMap[index4].tv;
 
            // Modify the texture coordinates to cover the top and right edges.
            if (tu  ==  0.0f) {tu  =  1.0f; }
            if (tv  ==  1.0f) {tv  =  0.0f; }
 
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            m_vertices[index].texture  =  XMFLOAT2(tu, tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
            index ++ ;
 
			// Bottom right.
            tu  =  m_heightMap[index2].tu;
 
            // Modify the texture coordinates to cover the right edge.
            if (tu  ==  0.0f) {tu  =  1.0f; }
 
            m_vertices[index].position  =  XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
            m_vertices[index].texture  =  XMFLOAT2(tu, m_heightMap[index2].tv);
            m_vertices[index].normal  =  XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
            index ++ ;            
        }
    }
 
    return  true ;
}
 
 
void  TerrainClass::ShutdownBuffers()
{
    // Release the vertex array.
    if (m_vertices)
    {
        delete  [] m_vertices;
        m_vertices  =  0 ;
    }
}
 
 
int  TerrainClass::GetVertexCount()
{
    return  m_vertexCount;
}
 
 
void  TerrainClass::CopyVertexArray( void *  vertexList)
{
    memcpy(vertexList, m_vertices,  sizeof (VertexType)  *  m_vertexCount);
}
