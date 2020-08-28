#include  "stdafx.h"
#include  "terrainclass.h"
#include  "frustumclass.h"
#include  "terrainshaderclass.h"
#include  "quadtreeclass.h"
 
 
QuadTreeClass::QuadTreeClass()
{
}
 
 
QuadTreeClass::QuadTreeClass( const  QuadTreeClass &  other)
{
}
 
 
QuadTreeClass::~QuadTreeClass()
{
}
 
 
bool  QuadTreeClass::Initialize( TerrainClass *  terrain, ID3D11Device *  device)
{
    float  centerX  =  0.0f;
    float  centerZ  =  0.0f;
    float  width  =  0.0f;
    
    // Get the number of vertices in the terrain vertex array.
    int  vertexCount  =  terrain -> GetVertexCount();
 
    // Store the total number of triangles in the vertex list.
    m_triangleCount  =  vertexCount  /  3 ;
 
    // Create a vertex array containing all terrain vertices.
    m_vertexList  =  new  VertexType[vertexCount];
    if ( ! m_vertexList)
    {
        return  false ;
    }
 
    // Copy the terrain vertices to the vertex list.
    terrain -> CopyVertexArray(( void * )m_vertexList);
 
    // Calculate the center x, z and width of the mesh
    CalculateMeshDimensions(vertexCount, centerX, centerZ, width);
 
    // Create the parent node of the quad tree.
    m_parentNode  =  new  NodeType;
    if ( ! m_parentNode)
    {
        return  false ;
    }
 
    // Build quad tree recursively based on vertex list data and mesh dimensions.
    CreateTreeNode(m_parentNode, centerX, centerZ, width, device);
 
    // Put the list of vertices because the quad tree has vertices at each node.
    if (m_vertexList)
    {
        delete  [] m_vertexList;
        m_vertexList  =  0 ;
    }
 
    return  true ;
}
 
 
void  QuadTreeClass::Shutdown()
{
    // Free the quad tree data recursively.
    if (m_parentNode)
    {
        ReleaseNode (m_parentNode);
        delete  m_parentNode;
        m_parentNode  =  0 ;
    }
}
 
 
void  QuadTreeClass::Render(FrustumClass *  frustum, ID3D11DeviceContext *  deviceContext,  TerrainShaderClass *  shader)
{
    // Initialize the number of triangles drawn for this frame.
    m_drawCount  =  0 ;
 
    // Render each visible node starting at the parent node and moving down the tree
    RenderNode(m_parentNode, frustum, deviceContext, shader);
}
 
 
int  QuadTreeClass::GetDrawCount()
{
    return  m_drawCount;
}
 
 
void  QuadTreeClass::CalculateMeshDimensions( int  vertexCount,  float &  centerX,  float &  centerZ,  float &  meshWidth)
{
    // Initialize the center position of the mesh to 0.
    centerX  =  0.0f;
    centerZ  =  0.0f;
 
    // Combine all the vertices of the mesh.
    for ( int  i = 0 ; i < vertexCount; i ++ )
    {
        centerX  +=  m_vertexList[i].position.x;
        centerZ  +=  m_vertexList[i].position.z;
    }
 
    // And divide by the number of vertices to find the midpoint of the mesh.
    centerX  =  centerX  /  ( float )vertexCount;
    centerZ  =  centerZ  /  ( float )vertexCount;
 
    // Initialize the maximum and minimum size of the mesh.
    float  maxWidth  =  0.0f;
    float  maxDepth  =  0.0f;
 
    float  minWidth  =  fabsf(m_vertexList[ 0 ].position.x  -  centerX);
    float  minDepth  =  fabsf(m_vertexList[ 0 ].position.z  -  centerZ);
 
    // Go through all vertices and find the maximum and minimum width and depth of the mesh.
    for ( int  i = 0 ; i < vertexCount; i ++ )
    {
        float  width  =  fabsf(m_vertexList[i].position.x  -  centerX);    
        float  depth  =  fabsf(m_vertexList[i].position.z  -  centerZ);    
 
        if (width  >  maxWidth) {maxWidth  =  width; }
        if (depth  >  maxDepth) {maxDepth  =  depth; }
        if (width  <  minWidth) {minWidth  =  width; }
        if (depth  <  minDepth) {minDepth  =  depth; }
    }
 
    // Find the absolute maximum between the minimum and maximum depth and width.
    float  maxX  =  ( float )max(fabs(minWidth), fabs(maxWidth));
    float  maxZ  =  ( float )max(fabs(minDepth), fabs(maxDepth));
    
    // Calculate the maximum diameter of the mesh.
    meshWidth  =  max(maxX, maxZ)  *  2.0f;
}
 
 
void  QuadTreeClass::CreateTreeNode(NodeType *  node,  float  positionX,  float  positionZ,  float  width, ID3D11Device *  device)
{
    // Save the location and size of the node.
    node -> positionX  =  positionX;
    node -> positionZ  =  positionZ;
    node -> width  =  width;
 
    // Initialize the number of triangles in the node to 0.
    node -> triangleCount  =  0 ;
 
    // Initialize the vertex and index buffers to null.
    node -> vertexBuffer  =  0 ;
    node -> indexBuffer  =  0 ;
 
    // Initialize the child nodes of this node to null.
    node -> nodes[ 0 ]  =  0 ;
    node -> nodes[ 1 ]  =  0 ;
    node -> nodes[ 2 ]  =  0 ;
    node -> nodes[ 3 ]  =  0 ;
 
    // Count the number of triangles in this node.
    int  numTriangles  =  CountTriangles (positionX, positionZ, width);
 
    // Case 1: If this node doesn't have a triangle, it goes back to an empty state and doesn't need to be processed.
    if (numTriangles  ==  0 )
    {
        return ;
    }
 
    // Case 2: If this node has too many triangles, split it into 4 smaller tree nodes of equal size.
    if (   numTriangles > MAX_TRIANGLES)
    {
        for ( int  i = 0 ; i < 4 ; i ++ )
        {
            // Calculate the position offset for the new child node.
            float  offsetX  =  (((i%  2 )  <  1 )?  - 1.0f:  1.0f)  *  (width  /  4.0f);
            float  offsetZ  =  (((i%  4 )  <  2 )?  - 1.0f:  1.0f)  *  (width  /  4.0f);
 
            // Check if the new node has a triangle.
            int  count  =  CountTriangles((positionX  +  offsetX), (positionZ  +  offsetZ), (width  /  2.0f));
            if (count  >  0 )
            {
                // If you have a triangle with this new node, create a child node.
                node -> nodes[i]  =  new  NodeType;
 
                // Now expand the tree starting at this new child node.
                CreateTreeNode(node -> nodes[i], (positionX  +  offsetX), (positionZ  +  offsetZ), (width  /  2.0f), device);
            }
        }
        return ;
    }
 
    // Case 3: If this node is not empty and its number of triangles is less than the maximum 
    // This node is at the bottom of the tree, so create a list of triangles to store.
    node -> triangleCount  =  numTriangles;
 
    // Calculate the number of vertices.
    int  vertexCount  =  numTriangles  *  3 ;
 
    // Create the vertex array.
    VertexType *  vertices  =  new  VertexType[vertexCount];
 
    // Create an indexed array.
    unsigned  long *  indices  =  new  unsigned  long [vertexCount];
 
    // Initialize the indices of this new vertex and index array.
    int  index  =  0 ;
    int  vertexIndex  =  0 ;
 
    // Go through all triangles in the vertex list.
    for ( int  i = 0 ; i < m_triangleCount; i ++ )
    {
        // If the triangle is inside this node, add it to the vertex array.
        if (IsTriangleContained(i, positionX, positionZ, width)  ==  true )
        {
            // Calculate the index into the list of terrain vertices.
            vertexIndex  =  i  *  3 ;
 
            // Get the three vertices of this triangle from the vertex list.
            vertices[index].position  =  m_vertexList[vertexIndex].position;
            vertices[index].texture  =  m_vertexList[vertexIndex].texture;
            vertices[index].normal  =  m_vertexList[vertexIndex].normal;
            indices[index]  =  index;
            index ++ ;
 
            vertexIndex ++ ;
            vertices[index].position  =  m_vertexList[vertexIndex].position;
            vertices[index].texture  =  m_vertexList[vertexIndex].texture;
            vertices[index].normal  =  m_vertexList[vertexIndex].normal;
            indices[index]  =  index;
            index ++ ;
 
            vertexIndex ++ ;
            vertices[index].position  =  m_vertexList[vertexIndex].position;
            vertices[index].texture  =  m_vertexList[vertexIndex].texture;
            vertices[index].normal  =  m_vertexList[vertexIndex].normal;
            indices[index]  =  index;
            index ++ ;
        }
    }
 
    // Set the structure of the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage  =  D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth  =  sizeof (VertexType)  *  vertexCount;
    vertexBufferDesc.BindFlags  =  D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags  =  0 ;
    vertexBufferDesc.MiscFlags  =  0 ;
    vertexBufferDesc.StructureByteStride  =  0 ;
 
    // Provide a pointer to the vertex data in the subresource structure.
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem  =  vertices;
    vertexData.SysMemPitch  =  0 ;
    vertexData.SysMemSlicePitch  =  0 ;
 
    // Now finally create the vertex buffer.
    device -> CreateBuffer( & vertexBufferDesc,  & vertexData,  & node -> vertexBuffer);
 
    // Set the description of the index buffer.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage  =  D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth  =  sizeof ( unsigned  long )  *  vertexCount;
    indexBufferDesc.BindFlags  =  D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags  =  0 ;
    indexBufferDesc.MiscFlags  =  0 ;
    indexBufferDesc.StructureByteStride  =  0 ;
 
    // Provide a pointer to the index data in the sub-resource structure.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem  =  indices;
    indexData.SysMemPitch  =  0 ;
    indexData.SysMemSlicePitch  =  0 ;
 
    // Create the index buffer.
    device -> CreateBuffer ( & indexBufferDesc,  & indexData,  & node -> indexBuffer);
 
    // Now that the data is stored in the node's buffer, free the vertex and index arrays.
    delete  [] vertices;
    vertices  =  0 ;
 
    delete  [] indices;
    indices  =  0 ;
}
 
 
int  QuadTreeClass::CountTriangles( float  positionX,  float  positionZ,  float  width)
{
    // Initialize the count to 0.
    int  count  =  0 ;
 
    // Go through all triangles in the entire mesh and see which nodes should be inside this node.
    for ( int  i = 0 ; i < m_triangleCount; i ++ )
    {
        // If the triangle is inside the node, increment it by 1.
        if (IsTriangleContained(i, positionX, positionZ, width)  ==  true )
        {
            count ++ ;
        }
    }
 
    return  count;
}
 
 
bool  QuadTreeClass::IsTriangleContained( int  index,  float  positionX,  float  positionZ,  float  width)
{
    // Calculate the radius of this node.
    float  radius  =  width  /  2.0f;
 
    // Get the index into the vertex list.
    int  vertexIndex  =  index  *  3 ;
 
    // Get the three vertices of this triangle from the vertex list.
    float  x1  =  m_vertexList[vertexIndex].position.x;
    float  z1  =  m_vertexList[vertexIndex].position.z;
    vertexIndex ++ ;
    
    float  x2  =  m_vertexList[vertexIndex].position.x;
    float  z2  =  m_vertexList[vertexIndex].position.z;
    vertexIndex ++ ;
 
    float  x3  =  m_vertexList[vertexIndex].position.x;
    float  z3  =  m_vertexList[vertexIndex].position.z;
 
    // Make sure the minimum value of the x-coordinate of the triangle is inside the node.
    float  minimumX  =  min(x1, min(x2, x3));
    if (minimumX  >  (positionX  +  radius))
    {
        return  false ;
    }
 
    // Check if the maximum value of the triangle's x coordinate is inside the node.
    float  maximumX  =  max(x1, max(x2, x3));
    if (maximumX  <  (positionX  -  radius))
    {
        return  false ;
    }
 
    // Make sure the minimum value of the z coordinate of the triangle is inside the node.
    float  minimumZ  =  min(z1, min(z2, z3));
    if (minimumZ  >  (positionZ  +  radius))
    {
        return  false ;
    }
 
    // Check if the maximum value of the z coordinate of the triangle is inside the node.
    float  maximumZ  =  max(z1, max(z2, z3));
    if (maximumZ  <  (positionZ  -  radius))
    {
        return  false ;
    }
 
    return  true ;
}
 
 
void  QuadTreeClass::ReleaseNode(NodeType *  node)
{
    // Recursively go down the tree and put the bottom node first.
    for ( int  i = 0 ; i < 4 ; i ++ )
    {
        if (node -> nodes[i]  !=  0 )
        {
            ReleaseNode(node -> nodes[i]);
        }
    }
 
    // Release this node's vertex buffer.
    if (node -> vertexBuffer)
    {
        node -> vertexBuffer -> Release();
        node -> vertexBuffer  =  0 ;
    }
 
    // Release this node's index buffer.
    if (node -> indexBuffer)
    {
        node -> indexBuffer -> Release();
        node -> indexBuffer  =  0 ;
    }
 
    // Release the four child nodes.
    for ( int  i = 0 ; i < 4 ; i ++ )
    {
        if (node -> nodes[i])
        {
            delete  node -> nodes[i];
            node -> nodes[i]  =  0 ;
        }
    }
}
 
 
void  QuadTreeClass::RenderNode(NodeType *  node, FrustumClass *  frustum, ID3D11DeviceContext *  deviceContext,
                               TerrainShaderClass *  shader)
{
    // Make sure the node can be seen and the height is not important in the quad tree.
    // If not visible, none of the children can continue down the tree and this is where the speed has increased.
    if ( ! frustum -> CheckCube (node -> positionX,  0.0F, node -> positionZ, (node -> width  /  2.0F)))
    {
        return ;
    }
 
    // If you can see it, check all four child nodes to see if you can see it.
    int  count  =  0 ;
    for ( int  i = 0 ; i < 4 ; i ++ )
    {
        if (node -> nodes[i]  !=  0 )
        {
            count ++ ;
            RenderNode(node -> nodes[i], frustum, deviceContext, shader);
        }
    }
 
    // If you have child nodes, there is no need to continue as the parent node does not contain a triangle to render.
    if (count  !=  0 )
    {
        return ;
    }
 
    // Otherwise, you can see this node, and if there is a triangle in it, render this triangle.
 
    // Set the vertex buffer stride and offset.
    unsigned  int  stride  =  sizeof (VertexType); 
    unsigned  int  offset  =  0 ;
 
    // Set the vertex buffer active in the input assembler so it can be rendered.
    deviceContext -> IASetVertexBuffers ( 0 ,  1 ,  & node -> vertexBuffer,  & stride,  & offset);
 
    // Set the index buffer active in the input assembler so it can be rendered.
    deviceContext -> IASetIndexBuffer(node -> indexBuffer, DXGI_FORMAT_R32_UINT,  0 );
 
    // Set the type of primitive that should be rendered in this vertex buffer. In this case, it is a triangle.
    deviceContext -> IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
    // Determine the number of indices on this node.
    int  indexCount  =  node -> triangleCount  *  3 ;
 
    // Call the terrain shader to render the polygon of this node.
    shader -> RenderShader(deviceContext, indexCount);
 
    // Increase the number of polygons rendered during this frame.
    m_drawCount  +=  node -> triangleCount;
}
