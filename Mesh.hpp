
#ifndef GLOW_MESH_HPP
#define GLOW_MESH_HPP

#include "Common.hpp"

class Mesh {
public:
    
    // TODO handle skinning weights and skeleton, or use a different class?
    
    bool load(std::string const & path);
    bool loadObj(std::string const & path);
    // TODO STL, PLY?
    
    void clear();
    
    GLint addVertex(glm::vec3 const & position);
    
    // Winding must be consistent over the whole mesh
    // Note: CCW winding means front-face
    GLint addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n1, glm::vec3 const & n2, glm::vec3 const & n3, glm::vec2 const & c1, glm::vec2 const & c2, glm::vec2 const & c3);
    GLint addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n1, glm::vec3 const & n2, glm::vec3 const & n3);
    GLint addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n);
    GLint addFace(GLint v1, GLint v2, GLint v3);
    
    // Vertices data
    GLint getCount() const;
    glm::vec3 const * getPositions() const;
    glm::vec3 const * getNormals() const;
    glm::vec2 const * getCoordinates() const;
    // TODO get element indices for adjacency
    
private:
    
    // TODO face/vertex normals?

    std::vector<glm::vec3> vertex_position;
    std::vector<GLint> vertex_halfedge;
    
    std::vector<GLint> face_halfedge;
    
    std::vector<glm::vec3> halfedge_position;
    std::vector<glm::vec3> halfedge_normal;
    std::vector<glm::vec2> halfedge_coordinate;
    std::vector<GLint> halfedge_next;
    std::vector<GLint> halfedge_opposite;
    std::vector<GLint> halfedge_vertex;
    std::vector<GLint> halfedge_face;
    
};

#endif
