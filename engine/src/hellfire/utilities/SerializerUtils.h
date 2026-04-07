//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <optional>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "hellfire/graphics/Vertex.h"
#include "nlohmann/json.hpp"

namespace hellfire {
    /// Binary I/O
    template<typename T>
    void write_binary(std::ostream &out, const T &val) {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        out.write(reinterpret_cast<const char *>(&val), sizeof(T));
    }

    template<typename T>
    bool read_binary(std::istream &in, T &val) {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        in.read(reinterpret_cast<char *>(&val), sizeof(T));
        return in.good();
    }

    template<typename T>
    void write_binary_vector(std::ostream &out, const std::vector<T> &vec) {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        const uint32_t size = static_cast<uint32_t>(vec.size());
        write_binary(out, size);
        if (size > 0) {
            out.write(reinterpret_cast<const char *>(vec.data()), size * sizeof(T));
        }
    }

    template<typename T>
    bool read_binary_vector(std::istream &in, std::vector<T> &vec) {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        uint32_t size;
        if (!read_binary(in, size)) return false;
        vec.resize(size);
        if (size > 0) {
            in.read(reinterpret_cast<char *>(vec.data()), size * sizeof(T));
        }
        return in.good();
    }

    inline void write_binary_string(std::ostream &out, const std::string &str) {
        const uint32_t length = static_cast<uint32_t>(str.size());
        write_binary(out, length);
        if (length > 0) {
            out.write(str.data(), length);
        }
    }

    inline bool read_binary_string(std::istream &in, std::string &str) {
        uint32_t length;
        if (!read_binary(in, length)) return false;
        str.resize(length);
        if (length > 0) {
            in.read(str.data(), length);
        }
        return in.good();
    }



    /// GLM Binary I/O
    inline void write_binary(std::ostream &out, const glm::vec2 &v) {
        out.write(reinterpret_cast<const char *>(&v.x), sizeof(float) * 2);
    }

    inline void write_binary(std::ostream &out, const glm::vec3 &v) {
        out.write(reinterpret_cast<const char *>(&v.x), sizeof(float) * 3);
    }

    inline void write_binary(std::ostream &out, const glm::vec4 &v) {
        out.write(reinterpret_cast<const char *>(&v.x), sizeof(float) * 4);
    }

    inline void write_binary(std::ostream &out, const glm::quat &q) {
        out.write(reinterpret_cast<const char *>(&q.x), sizeof(float) * 4);
    }

    inline void write_binary(std::ostream &out, const glm::mat4 &m) {
        out.write(reinterpret_cast<const char *>(&m[0][0]), sizeof(float) * 16);
    }

    inline void write_binary(std::ostream &out, const glm::mat3 &m) {
        out.write(reinterpret_cast<const char *>(&m[0][0]), sizeof(float) * 9);
    }

    inline bool read_binary(std::istream &in, glm::vec2 &v) {
        in.read(reinterpret_cast<char *>(&v.x), sizeof(float) * 2);
        return in.good();
    }

    inline bool read_binary(std::istream &in, glm::vec3 &v) {
        in.read(reinterpret_cast<char *>(&v.x), sizeof(float) * 3);
        return in.good();
    }

    inline bool read_binary(std::istream &in, glm::vec4 &v) {
        in.read(reinterpret_cast<char *>(&v.x), sizeof(float) * 4);
        return in.good();
    }

    inline bool read_binary(std::istream &in, glm::quat &q) {
        in.read(reinterpret_cast<char *>(&q.x), sizeof(float) * 4);
        return in.good();
    }

    inline bool read_binary(std::istream &in, glm::mat4 &m) {
        in.read(reinterpret_cast<char *>(&m[0][0]), sizeof(float) * 16);
        return in.good();
    }

    inline bool read_binary(std::istream &in, glm::mat3 &m) {
        in.read(reinterpret_cast<char *>(&m[0][0]), sizeof(float) * 9);
        return in.good();
    }

    /// Vertex I/O
    inline void write_binary(std::ostream& out, const Vertex& v) {
        write_binary(out, v.position);
        write_binary(out, v.normal);
        write_binary(out, v.texCoords);
        write_binary(out, v.color);
        write_binary(out, v.tangent);
        write_binary(out, v.bitangent);
    }

    inline bool read_binary(std::istream& in, Vertex& v) {
        if (!read_binary(in, v.position)) return false;
        if (!read_binary(in, v.normal)) return false;
        if (!read_binary(in, v.texCoords)) return false;
        if (!read_binary(in, v.color)) return false;
        if (!read_binary(in, v.tangent)) return false;
        if (!read_binary(in, v.bitangent)) return false;
        return true;
    }

    // Vector of vertices (can't use the template version)
    inline void write_vertex_vector(std::ostream& out, const std::vector<Vertex>& vec) {
        const uint32_t size = static_cast<uint32_t>(vec.size());
        write_binary(out, size);
        for (const auto& v : vec) {
            write_binary(out, v);
        }
    }

    inline bool read_vertex_vector(std::istream& in, std::vector<Vertex>& vec) {
        uint32_t size;
        if (!read_binary(in, size)) return false;
        vec.resize(size);
        for (auto& v : vec) {
            if (!read_binary(in, v)) return false;
        }
        return true;
    }

    /// JSON Helpers
    inline std::optional<glm::vec2> json_get_vec2(const nlohmann::json &j, const std::string &key) {
        if (!j.contains(key) || !j[key].is_array() || j[key].size() < 2) {
            return std::nullopt;
        }
        return glm::vec2(j[key][0].get<float>(), j[key][1].get<float>());
    }

    inline std::optional<glm::vec3> json_get_vec3(const nlohmann::json &j, const std::string &key) {
        if (!j.contains(key) || !j[key].is_array() || j[key].size() < 3) {
            return std::nullopt;
        }
        return glm::vec3(
            j[key][0].get<float>(),
            j[key][1].get<float>(),
            j[key][2].get<float>()
        );
    }

    inline std::optional<glm::vec3> json_to_vec3(const nlohmann::json &j) {
        if (!j.is_array() || j.size() != 3) return std::nullopt;

        for (const auto &elem: j) {
            if (!elem.is_number()) return std::nullopt;
        }

        return glm::vec3(j[0].get<float>(), j[1].get<float>(), j[2].get<float>());
    }

    inline std::optional<glm::vec4> json_get_vec4(const nlohmann::json &j, const std::string &key) {
        if (!j.contains(key) || !j[key].is_array() || j[key].size() < 4) {
            return std::nullopt;
        }
        return glm::vec4(
            j[key][0].get<float>(),
            j[key][1].get<float>(),
            j[key][2].get<float>(),
            j[key][3].get<float>()
        );
    }

    inline nlohmann::json vec2_to_json(const glm::vec2 &v) {
        return {v.x, v.y};
    }

    inline nlohmann::json vec3_to_json(const glm::vec3 &v) {
        return {v.x, v.y, v.z};
    }

    inline nlohmann::json vec4_to_json(const glm::vec4 &v) {
        return {v.x, v.y, v.z, v.w};
    }


    /// File Header Validation
    struct FileHeader {
        uint32_t magic;
        uint32_t version;
    };

    inline bool write_header(std::ostream &out, uint32_t magic, uint32_t version) {
        write_binary(out, magic);
        write_binary(out, version);
        return out.good();
    }

    inline bool read_and_validate_header(std::istream &in, uint32_t expected_magic, uint32_t max_version,
                                         uint32_t &out_version) {
        uint32_t magic;
        if (!read_binary(in, magic) || magic != expected_magic) {
            return false;
        }

        if (!read_binary(in, out_version) || out_version > max_version) {
            return false;
        }

        return true;
    }
} // namespace hellfire

namespace glm {
    /// GLM Helper extensions
    inline void to_json(nlohmann::json &j, const vec3 &v) {
        j = {v.x, v.y, v.z};
    }

    inline void from_json(const nlohmann::json &j, vec3 &v) {
        v = {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
    }
} // namespace glm
