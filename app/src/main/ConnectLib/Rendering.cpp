//
// Created by cheerwizard on 02.12.24.
//

#include "Rendering.hpp"

Shader::Shader(const char* filepath) {
    vector<ShaderSource> sources = readSources(filepath);

    if (sources.empty()) return;

    init();

    for (const auto& source : sources) {
        if (!compile(source)) {
            free();
            return;
        }
    }

    if (!link()) {
        free();
    }
}

Shader::~Shader() {
    free();
}

string Shader::readWithIncludes(const string& filepath) {
    string include = "#include ";

    static bool recursive = false;
    string fullSrc;
    ifstream file;

    file.open(filepath);

    if (!file.is_open()) {
        LOG_ERR("%s shader failed to open!", filepath);
        return fullSrc;
    }

    string lineBuffer;
    while (getline(file, lineBuffer)) {
        // Look for the new mShader include identifier
        if (lineBuffer.find(include) != lineBuffer.npos) {
            // Remove the include identifier, this will cause the path to remain
            lineBuffer.erase(0, include.size());

            // The include path is relative to the current mShader file path
            string pathOfThisFile;
            FileManager::getFilepath(filepath, pathOfThisFile);
            lineBuffer.insert(0, pathOfThisFile);

            // By using recursion, the new include file can be extracted
            // and inserted at this location in the mShader source code
            recursive = true;
            fullSrc += readWithIncludes(lineBuffer) + '\n';

            // Do not add this line to the mShader source code, as the include
            // path would generate a compilation issue in the final source code
            continue;
        }

        fullSrc += lineBuffer + '\n';
    }

    // Only add the null terminator at the end of the complete file,
    // essentially skipping recursive function calls this way
    if (!recursive) {
        fullSrc += '\0';
    }

    file.close();

    return fullSrc;
}

vector<ShaderSource> Shader::readSources(const string &filepath) {
    string source = readWithIncludes(filepath);

    if (source.empty()) {
        return {};
    }

    static const unordered_map<string, ShaderStage> stages = {
            { "#type vertex", ShaderStage_VERTEX },
            { "#type pixel", ShaderStage_PIXEL },
            { "#type geometry", ShaderStage_GEOMETRY },
            { "#type tesselation", ShaderStage_TESSELATION },
            { "#type compute", ShaderStage_COMPUTE },
    };

    vector<ShaderSource> sources;

    for (const auto& stageEntry : stages) {
        const string& stageID = stageEntry.first;
        const ShaderStage stage = stageEntry.second;
        size_t stagePos = source.find(stageID);
        if (stagePos != string::npos) {
            source = source.erase(stagePos, stageID.size());
            size_t nextStagePos = source.find("#type");
            string stageSource = source.substr(0, nextStagePos);
            sources.emplace_back(stage, stageSource);
        }
    }

    return sources;
}