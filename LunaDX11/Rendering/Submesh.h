#pragma once

#include <d3d11.h>
#include <type_traits>

struct Submesh
{
    Submesh() = default;

    template <typename T1, typename T2, typename T3>
    Submesh(T1 newIndexCount, T2 newStartIndexLocation, T3 newBaseVertexLocation)
    {
        static_assert(std::is_integral_v<T1>, "T1은 정수 타입이어야 합니다.");
        static_assert(std::is_integral_v<T2>, "T2은 정수 타입이어야 합니다.");
        static_assert(std::is_integral_v<T3>, "T3은 정수 타입이어야 합니다.");

        indexCount = static_cast<UINT>(newIndexCount);
        startIndexLocation = static_cast<UINT>(newStartIndexLocation);
        baseVertexLocation = static_cast<INT>(newBaseVertexLocation);
    }

    UINT indexCount;
    UINT startIndexLocation;
    INT baseVertexLocation;
};
