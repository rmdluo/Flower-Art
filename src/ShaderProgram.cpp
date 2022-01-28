#include <ShaderProgram.hpp>

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragPath) {
    const char* vertexCode;
    const char* fragCode;

    std::string vertexStr;
    std::string fragStr;

    std::ifstream vertexFile;
    std::ifstream fragFile;

    vertexFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream vertexStream;
    std::stringstream fragStream;

    try
    {
        vertexFile.open(vertexPath);
        fragFile.open(fragPath);

        vertexStream << vertexFile.rdbuf();
        fragStream << fragFile.rdbuf();

        vertexFile.close();
        fragFile.close();

        vertexStr = vertexStream.str();
        fragStr = fragStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    vertexCode = vertexStr.c_str();
    fragCode = fragStr.c_str();

    int success;
    char infoLog[512];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        std::cout << vertexStream.str() << std::endl;

        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    unsigned int fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        std::cout << fragStream.str() << std::endl;

        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

void ShaderProgram::use() {
    glUseProgram(program);
}

void ShaderProgram::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value); 
}

void ShaderProgram::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value); 
}

void ShaderProgram::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value); 
}