
#include "Mesh.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

bool Mesh::load(std::string const & path) {
    return loadObj(path);
}

bool Mesh::loadObj(std::string const & path) {
    clear();
    FILE * file = fopen(path.c_str(), "r");
    if (!file)
        return false;
    char line[1024];
    char tag[1024];
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coordinates;
    while (fgets(line, sizeof(line), file))
        if (sscanf(line, "%s", tag) == 1) {
            if (strcmp(tag, "v") == 0) {
                glm::vec3 p;
                sscanf(line, "%*s %f %f %f", &p.x, &p.y, &p.z);
                addVertex(p);
            }
            else if (strcmp(tag, "vt") == 0) {
                glm::vec2 c;
                sscanf(line, "%*s %f %f", &c.x, &c.y);
                coordinates.push_back(c);
            }
            else if (strcmp(tag, "vn") == 0) {
                glm::vec3 n;
                sscanf(line, "%*s %f %f %f", &n.x, &n.y, &n.z);
                normals.push_back(n);
            }
            else if (strcmp(tag, "f") == 0) {
                std::vector<glm::ivec3> indices;
                strtok(line, " \t");
                while (char * part = strtok(NULL, " \t")) {
                    glm::ivec3 indice(-1, -1, -1);
                    char * s1 = strchr(part, '/');
                    if (s1) {
                        char * s2 = strchr(s1 + 1, '/');
                        if (s2) {
                            indice.z = strtol(s2 + 1, NULL, 10) - 1;
                            *s2 = '\0';
                        }
                        indice.y = strtol(s1 + 1, NULL, 10) - 1;
                        *s1 = '\0';
                    }
                    indice.x = strtol(part, NULL, 10) - 1;
                    indices.push_back(indice);
                }
                for (size_t i = 2; i < indices.size(); ++i) {
                    GLint p1 = indices[0].x;
                    GLint p2 = indices[i - 1].x;
                    GLint p3 = indices[i].x;
                    glm::vec2 c(0, 0);
                    glm::vec2 c1 = indices[0].y >= 0 ? coordinates[indices[0].y] : c;
                    glm::vec2 c2 = indices[i - 1].y >= 0 ? coordinates[indices[i - 1].y] : c;
                    glm::vec2 c3 = indices[i].y >= 0 ? coordinates[indices[i].y] : c;
                    glm::vec3 n = glm::normalize(glm::cross(vertex_position[p2] - vertex_position[p1], vertex_position[p3] - vertex_position[p1]));
                    glm::vec3 n1 = indices[0].z >= 0 ? normals[indices[0].z] : n;
                    glm::vec3 n2 = indices[i - 1].z >= 0 ? normals[indices[i - 1].z] : n;
                    glm::vec3 n3 = indices[i].z >= 0 ? normals[indices[i].z] : n;
                    addFace(p1, p2, p3, n1, n2, n3, c1, c2, c3);
                }
            }
        }
    fclose(file);
    return true;
}

void Mesh::clear() {
    vertex_position.clear();
    vertex_halfedge.clear();
    face_halfedge.clear();
    halfedge_position.clear();
    halfedge_normal.clear();
    halfedge_coordinate.clear();
    halfedge_next.clear();
    halfedge_opposite.clear();
    halfedge_vertex.clear();
    halfedge_face.clear();
}

GLint Mesh::addVertex(glm::vec3 const & position) {
    GLint index = vertex_position.size();
    vertex_position.push_back(position);
    vertex_halfedge.push_back(-1);
    return index;
}

GLint Mesh::addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n1, glm::vec3 const & n2, glm::vec3 const & n3, glm::vec2 const & c1, glm::vec2 const & c2, glm::vec2 const & c3) {
    GLint index = face_halfedge.size();
    GLint h1 = halfedge_position.size();
    GLint h2 = h1 + 1;
    GLint h3 = h1 + 2;
    GLint o1 = -1;
    GLint o2 = -1;
    GLint o3 = -1;
    for (GLint h = 0; h < h1; ++h) {
        // TODO properly handle incoherent winding (maybe ignore and provide a method to fix this)
        assert(halfedge_vertex[h] != v1 || halfedge_vertex[halfedge_next[h]] != v2);
        assert(halfedge_vertex[h] != v2 || halfedge_vertex[halfedge_next[h]] != v3);
        assert(halfedge_vertex[h] != v3 || halfedge_vertex[halfedge_next[h]] != v1);
        if (halfedge_vertex[h] == v2 && halfedge_vertex[halfedge_next[h]] == v1)
            o1 = h;
        if (halfedge_vertex[h] == v3 && halfedge_vertex[halfedge_next[h]] == v2)
            o2 = h;
        if (halfedge_vertex[h] == v1 && halfedge_vertex[halfedge_next[h]] == v3)
            o3 = h;
    }
    halfedge_position.push_back(vertex_position[v1]);
    halfedge_position.push_back(vertex_position[v2]);
    halfedge_position.push_back(vertex_position[v3]);
    halfedge_normal.push_back(n1);
    halfedge_normal.push_back(n2);
    halfedge_normal.push_back(n3);
    halfedge_coordinate.push_back(c1);
    halfedge_coordinate.push_back(c2);
    halfedge_coordinate.push_back(c3);
    halfedge_next.push_back(h2);
    halfedge_next.push_back(h3);
    halfedge_next.push_back(h1);
    halfedge_opposite.push_back(o1);
    halfedge_opposite.push_back(o2);
    halfedge_opposite.push_back(o3);
    halfedge_vertex.push_back(v1);
    halfedge_vertex.push_back(v2);
    halfedge_vertex.push_back(v3);
    halfedge_face.push_back(index);
    halfedge_face.push_back(index);
    halfedge_face.push_back(index);
    face_halfedge.push_back(h1);
    return index;
}

GLint Mesh::addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n1, glm::vec3 const & n2, glm::vec3 const & n3) {
    glm::vec2 c(0.0, 0.0);
    return addFace(v1, v2, v3, n1, n2, n3, c, c, c);
}

GLint Mesh::addFace(GLint v1, GLint v2, GLint v3, glm::vec3 const & n) {
    return addFace(v1, v2, v3, n, n, n);
}

GLint Mesh::addFace(GLint v1, GLint v2, GLint v3) {
    glm::vec3 n = glm::normalize(glm::cross(vertex_position[v2] - vertex_position[v1], vertex_position[v3] - vertex_position[v1]));
    return addFace(v1, v2, v3, n);
}

GLint Mesh::getCount() const {
    return halfedge_position.size();
}

glm::vec3 const * Mesh::getPositions() const {
    return halfedge_position.data();
}

glm::vec3 const * Mesh::getNormals() const {
    return halfedge_normal.data();
}

glm::vec2 const * Mesh::getCoordinates() const {
    return halfedge_coordinate.data();
}
