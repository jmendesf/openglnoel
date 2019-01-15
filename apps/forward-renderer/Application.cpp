#include "Application.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/GLProgram.hpp>

int Application::run()
{
    glClearColor(1,0,0,1);

	// Put here code to run before rendering loop
    const auto program = glmlv::compileProgram({m_ShadersRootPath / "forward-renderer" / "forward.vs.glsl" ,
                                                m_ShadersRootPath / "forward-renderer" / "forward.fs.glsl" });
    program.use();

    GLint uMVPMatrix = program.getUniformLocation("uMVPMatrix");
    GLint uMVMatrix = program.getUniformLocation("uMVMatrix");
    GLint uNormalMatrix = program.getUniformLocation("uNormalMatrix");

    ProjMatrix = glm::perspective(glm::radians(70.f),1280.0f/720.0f,0.1f,100.0f);
    MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    glEnable(GL_DEPTH_TEST);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
       glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
       glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
       glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        const auto fbSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, fbSize.x, fbSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sg.indexBuffer.size(), GL_UNSIGNED_INT,  (void*)0 );

        // GUI code:
		glmlv::imguiNewFrame();
        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        glmlv::imguiRenderFrame();

        glfwPollEvents(); // Poll for and process events

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            // Put here code to handle user interactions
        }

		m_GLFWHandle.swapBuffers(); // Swap front and back buffers

    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    // Put here initialization code

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glGenVertexArrays(1, &vao);

    sg = glmlv::makeCube();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferStorage(GL_ARRAY_BUFFER, sg.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f),
                 sg.vertexBuffer.data(), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sg.indexBuffer.size()*sizeof(uint32_t),
                 sg.indexBuffer.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
                          (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
                          (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f),
                          (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);
}
