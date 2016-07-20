
#include "Scene.hpp"

Scene::Scene(GLFWwindow * window) : window(window) {}

bool Scene::initialize() {
    // TODO handle errors
    
    // Get screen size
    glfwGetFramebufferSize(window, &width, &height);
    
    // Initialize matrices
    projection = glm::perspective(PI / 3.0f, (float)width / (float)height, 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(-2.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Add some lights
    lights.push_back({{1, 1, 2}, {1, 0.8, 0.2}, 5});
    lights.push_back({{-1, 0, 3}, {0.2, 0.4, 1}, 5});
    
    // Load texture
    image.load("Test.bmp");
    texture.bind(0);
    texture.create(image);
    texture.setInterpolation(true);
    
    // Load cube mesh
    cube_mesh.load("Cube.obj");
    cube_buffer.bind(GL_ARRAY_BUFFER);
    cube_buffer.setData(cube_mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
    cube_buffer.setSubData(0, cube_mesh.getCount() * 4 * 3, cube_mesh.getPositions());
    cube_buffer.setSubData(cube_mesh.getCount() * 4 * 3, cube_mesh.getCount() * 4 * 3, cube_mesh.getNormals());
    cube_buffer.setSubData(cube_mesh.getCount() * 4 * (3 + 3), cube_mesh.getCount() * 4 * 2, cube_mesh.getCoordinates());
    cube_array.bind();
    cube_array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    cube_array.addAttribute(1, 3, GL_FLOAT, 0, cube_mesh.getCount() * 4 * 3);
    cube_array.addAttribute(2, 2, GL_FLOAT, 0, cube_mesh.getCount() * 4 * (3 + 3));

    // Load square mesh
    square_mesh.load("Square.obj");
    square_buffer.bind(GL_ARRAY_BUFFER);
    square_buffer.setData(square_mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
    square_buffer.setSubData(0, square_mesh.getCount() * 4 * 3, square_mesh.getPositions());
    square_buffer.setSubData(square_mesh.getCount() * 4 * 3, square_mesh.getCount() * 4 * 3, square_mesh.getNormals());
    square_buffer.setSubData(square_mesh.getCount() * 4 * (3 + 3), square_mesh.getCount() * 4 * 2, square_mesh.getCoordinates());
    square_array.bind();
    square_array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    square_array.addAttribute(1, 3, GL_FLOAT, 0, square_mesh.getCount() * 4 * 3);
    square_array.addAttribute(2, 2, GL_FLOAT, 0, square_mesh.getCount() * 4 * (3 + 3));
    
    // Load depth-only rendering shader
    depth_shader.addSourceFile(GL_VERTEX_SHADER, "Depth.vs");
    depth_shader.addSourceFile(GL_FRAGMENT_SHADER, "Depth.fs");
    depth_shader.link();

    // Load shadow volume extrusion shader
    extrusion_shader.addSourceFile(GL_VERTEX_SHADER, "Extrusion.vs");
    extrusion_shader.addSourceFile(GL_GEOMETRY_SHADER, "Extrusion.gs");
    extrusion_shader.addSourceFile(GL_FRAGMENT_SHADER, "Extrusion.fs");
    extrusion_shader.link();

    // Load shading shader
    shading_shader.addSourceFile(GL_VERTEX_SHADER, "Shading.vs");
    shading_shader.addSourceFile(GL_FRAGMENT_SHADER, "Shading.fs");
    shading_shader.link();

    // Load texture shader
    texture_shader.addSourceFile(GL_VERTEX_SHADER, "Texture.vs");
    texture_shader.addSourceFile(GL_FRAGMENT_SHADER, "Texture.fs");
    texture_shader.link();
    
    return true;
}

void Scene::update() {
    float time = glfwGetTime();
    view = glm::lookAt(glm::vec3(glm::cos(time / 3) * 3.0f, glm::sin(time / 3) * 3.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ...
}

void Scene::render() {
    
    // Clear everything
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Enable depth test for geaometry rendering
    glEnable(GL_DEPTH_TEST);
    // TODO might be able to enable face culling at some point?
    
    // Select depth shader
    depth_shader.use();
    depth_shader.setUniform("projection", projection);
    depth_shader.setUniform("view", view);
    
    // Draw geometry
    drawUntexturedObjects(depth_shader);

    // Do not override old depth
    glDepthMask(GL_FALSE);

    // Enable stencil to render shadows
    glEnable(GL_STENCIL_TEST);

    // For each light...
    for (Light & light : lights) {

        // Clear stencil
        glClear(GL_STENCIL_BUFFER_BIT);

        // Use Carmack's reverse shadow volume strategy
        glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~(GLint)0);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        // Do not write color as well
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        // Select extrusion shader
        extrusion_shader.use();
        extrusion_shader.setUniform("projection", projection);
        extrusion_shader.setUniform("view", view);
        extrusion_shader.setUniform("light_position", light.position);
        extrusion_shader.setUniform("light_radius", light.radius);

        // TODO depth clamp?
        // see https://www.opengl.org/wiki_132/index.php?title=Vertex_Post-Processing&redirect=no#Depth_clamping

        // Draw geometry
        drawCasterObjects(extrusion_shader);

        // Now, write color
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Use stencil to only draw on non-zero area
        glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0, ~(GLint)0);
        glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

        // Render only fragments that are exactly on geometry
        glDepthFunc(GL_EQUAL);

        // Use additive blend to combine lightmaps
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        // Select shading shader
        shading_shader.use();
        shading_shader.setUniform("projection", projection);
        shading_shader.setUniform("view", view);
        shading_shader.setUniform("light_position", light.position);
        shading_shader.setUniform("light_color", light.color);
        shading_shader.setUniform("light_radius", light.radius);

        // Draw geometry again to shade surfaces properly
        drawUntexturedObjects(shading_shader);

        // Restore default values
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
    }

    // Disable stencil
    glDisable(GL_STENCIL_TEST);

    // Render only fragments that are exactly on geometry
    glDepthFunc(GL_EQUAL);

    // Use multiplicative blend to combine light and texture
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);

    // Select texture shader
    texture_shader.use();
    texture_shader.setUniform("projection", projection);
    texture_shader.setUniform("view", view);
    texture_shader.setUniform("texture", 0);

    // Draw geometry again zo have textures
    drawTexturedObjects(texture_shader);

    // Restore defaults
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
}

void Scene::drawUntexturedObjects(Shader & shader) {
    // later, this will use game object structure to render only relevant meshes
    glm::mat4 model = glm::mat4();
    shader.setUniform("model", model); \
    cube_array.bind(); \
    glDrawArrays(GL_TRIANGLES, 0, cube_mesh.getCount()); \
    shader.setUniform("model", model); \
    square_array.bind(); \
    glDrawArrays(GL_TRIANGLES, 0, square_mesh.getCount());
}

void Scene::drawCasterObjects(Shader & shader) {
    // later, this will not render details, like particles
    drawUntexturedObjects(shader);
}

void Scene::drawTexturedObjects(Shader & shader) {
    // later, this will have to bind specific texture for each object (or use an atlas/texture array)
    texture.bind(0);
    drawUntexturedObjects(shader);
}
