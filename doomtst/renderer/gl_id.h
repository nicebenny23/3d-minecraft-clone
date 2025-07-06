

#pragma once
#include <glad/glad.h>
#include "GlUtil.h"
namespace Gl {
    struct gl_id {
        GLuint id = 0;
        explicit gl_id(GLuint id_) : id(id_) {}

        bool IsValid() const { return id != 0; }
        GLuint Get() const { return id; }

        explicit operator GLuint() const = delete;
        bool operator==(const gl_id& other) const {
            return id == other.id;
        }
        bool operator!=(const gl_id& other) const {
            return id != other.id;
        }
    };
}