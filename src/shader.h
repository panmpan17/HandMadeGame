
typedef unsigned int GLuint;

class Shader
{
public:
    // Shader(const char* vertex_shader_text, const char* fragment_shader_text);
    Shader();
    ~Shader();

    GLuint getProgram() const { return m_nProgram; }

private:
    GLuint m_nProgram;
};