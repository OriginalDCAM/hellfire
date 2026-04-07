#pragma once

namespace DCraft
{
    class Material
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
}
