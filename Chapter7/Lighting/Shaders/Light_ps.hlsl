#include "SharedTypes.hlsli"

void ComputeCartoonDirectionalLight(Material material, DirectionalLight light, float3 normal, float3 toEyeDirection, out float4 ambient, out float4 diffuse, out float4 specular)
{
    // 결과들을 초기화한다.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ambient = material.ambient * light.ambient; // 엠비언트 값을 구한다.

    // 디퓨즈 계수는 표면과 수직일때 1로 가장 강하다. 90도 이상이 되면 0이된다.
    float diffuseFactor = dot(-light.direction, normal); // 계산의 편의를 위해 빛 방향의 반대 방향 벡터를 사용한다.
    diffuseFactor = 0.4f * step(diffuseFactor, 0.0f) + 0.6f * (step(0.0f, diffuseFactor) - step(0.5f, diffuseFactor)) + step(0.5f, diffuseFactor);
    diffuse = diffuseFactor * material.diffuse * light.diffuse; // 디퓨즈 값을 구한다.

    const float3 reflectDirection = reflect(light.direction, normal); // 노멀에 대한 반사 벡터를 구한다.
    float specularFactor = pow(max(dot(reflectDirection, toEyeDirection), 0.0f), material.specular.w); // 스페큘러 계수를 구한다.
    if (specularFactor < 0.0f)
    {
        return;
    }

    specularFactor = 0.4f * step(specularFactor, 0.1f) + 0.5f * (step(0.1f, specularFactor) - step(0.8f, specularFactor)) + 0.8f * step(0.8f, specularFactor);
    specular = specularFactor * material.specular * light.specular; // 스페큘러 값을 구한다.
}

void ComputeCartoonPointLight(Material material, PointLight light, float3 position, float3 normal, float3 toEyeDirection, out float4 ambient, out float4 diffuse, out float4 specular)
{
    // 결과들을 초기화한다.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    const float3 toLightVector = light.position - position; // 빛을 향하는 벡터를 구한다.
    const float toLightDistance = length(toLightVector); // 빛을 향하는 길이를 구한다.
    if (toLightDistance > light.range) return; // 범위 밖에 있다면 빛을 아예 받지 않는다고 가정하기에 조기 리턴한다.

    const float3 lightDirection = -(toLightVector / toLightDistance); // 미리 구해둔 거리를 재활용해 노멀라이즈하고 방향을 바꿔 빛을 향하는 방향에서 빛이 나아가는 방향으로 바꾼다.
    float diffuseFactor = dot(-lightDirection, normal); // 편의를 위해 빛 방향의 반대 방향 벡터를 사용한다.
    diffuseFactor = 0.4f * step(diffuseFactor, 0.0f) + 0.6f * (step(0.0f, diffuseFactor) - step(0.5f, diffuseFactor)) + step(0.5f, diffuseFactor);

    ambient = material.ambient * light.ambient; // 앰비언트 값을 구한다.

    diffuse = diffuseFactor * material.diffuse * light.diffuse; // 디퓨즈 값을 구한다.

    const float3 reflectDirection = reflect(lightDirection, normal); // 노멀에 대한 반사 방향을 구한다.
    float specularFactor = pow(max(dot(reflectDirection, toEyeDirection), 0.0f), material.specular.w); // 스페큘러 계수를 구한다.
    specularFactor = 0.4f * step(specularFactor, 0.1f) + 0.5f * (step(0.1f, specularFactor) - step(0.8f, specularFactor)) + 0.8f * step(0.8f, specularFactor);
    specular = specularFactor * material.specular * light.specular; // 스페큘러 값을 구한다.

    const float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, toLightDistance, toLightDistance * toLightDistance)); // 어뉴테이션 값을 구한다.

    // 어뉴테이션을 적용한다.
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
}

void ComputeCartoonSpotLight(Material material, SpotLight light, float3 position, float3 normal, float3 toEyeDirection, out float4 ambient, out float4 diffuse, out float4 specular)
{
    // 결과들을 초기화한다.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    const float3 toLightVector = light.position - position;
    const float toLightDistance = length(toLightVector);
    if (toLightDistance > light.range) return; // 범위 밖에 있다면 빛을 아예 받지 않는다고 가정하기에 조기 리턴한다.

    ambient = material.ambient * light.ambient; // 앰비언트 값을 구한다.

    const float3 toLightDirection = toLightVector / toLightDistance; // 빛을 향하는 방향을 구한다.
    float diffuseFactor = dot(toLightDirection, normal);
    if (diffuseFactor <= 0.0f) return; // 빛을 받지 않는다면 조기 리턴한다.

    diffuseFactor = 0.4f * step(diffuseFactor, 0.0f) + 0.6f * (step(0.0f, diffuseFactor) - step(0.5f, diffuseFactor)) + step(0.5f, diffuseFactor);
    diffuse = diffuseFactor * material.diffuse * light.diffuse;

    const float3 reflectDirection = reflect(-toLightDirection, normal);
    float specularFactor = pow(max(dot(reflectDirection, toEyeDirection), 0.0f), material.specular.w);
    specularFactor = 0.4f * step(specularFactor, 0.1f) + 0.5f * (step(0.1f, specularFactor) - step(0.8f, specularFactor)) + 0.8f * step(0.8f, specularFactor);
    specular = specularFactor * material.specular * light.specular;

    const float spot = pow(max(dot(-toLightDirection, light.direction), 0.0f), light.spot); // 스팟 라이트는 라이트 자체가 바라보는 방향이 별개로 존재한다.
    const float attenuation = spot / dot(light.attenuation, float3(1.0f, toLightDistance, toLightDistance * toLightDistance)); // 어뉴테이션 값을 구한다.

    ambient *= spot;
    diffuse *= attenuation;
    specular *= attenuation;
}

float4 PS_Main(VertexOut input) : SV_Target
{
    input.normalWS = normalize(input.normalWS);

    const float3 toEyeWS = normalize(eyeWorldPosition - input.positionWS);

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 outAmbient, outDiffuse, outSpecular;

    ComputeCartoonDirectionalLight(material, directionalLight, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    ComputeCartoonPointLight(material, pointLight, input.positionWS, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    ComputeCartoonSpotLight(material, spotLight, input.positionWS, input.normalWS, toEyeWS, outAmbient, outDiffuse, outSpecular);
    ambient += outAmbient;
    diffuse += outDiffuse;
    specular += outSpecular;

    float4 litColor = ambient + diffuse + specular;
    litColor.a = material.diffuse.a;
    return litColor;
}
