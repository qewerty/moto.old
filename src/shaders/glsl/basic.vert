uniform vec3 lightPosition;
varying float lightIntensity;

int main()
{
    vec3 position       = vec3(gl_ModelViewMatrix * gl_Vertex);
    vec3 normal         = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightVec       = normalize(lightPosition - position);
    vec3 reflectVec     = reflect(-lightVec, normal);
    vec3 viewVec        = normalize(-position);

    float diffuse       = max(dot(lightVec, normal), 0.0);

    float spec = max(dot(reflectVec, viewVec), 0.0);
    spec = pow(spec, 16.0);

    lightIntensity = Kd*diffuse + Ks*spec;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
