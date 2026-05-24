#include "backrooms.hpp"
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <iostream>

const char* vsCode = R"(
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;
uniform mat4 mvp;
uniform mat4 matModel;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragPosition;
out vec3 fragNormal;
void main() {
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    fragNormal = normalize(normalMatrix * vertexNormal);
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)";

const char* fsCode = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform int flashlightOn;
uniform vec3 spotlightDir;
out vec4 finalColor;
void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDir = normalize(viewPos - fragPosition);
    float distance = length(viewPos - fragPosition);
    
    // Extreme dark ambient with some randomly placed lights
    float ambient = 0.01;
    float spotLightEffect = (sin(fragPosition.x * 0.8) + cos(fragPosition.z * 0.8)) * 0.5;
    spotLightEffect = smoothstep(0.7, 1.0, spotLightEffect); // Only illuminate some peaks
    ambient += spotLightEffect * 0.6; // Add light blobs
    
    float diffuse = 0.0;
    if (flashlightOn == 1) {
        float theta = dot(lightDir, normalize(-spotlightDir));
        if (theta > 0.85) { // Cutoff angle for flashlight
            float intensity = smoothstep(0.85, 0.95, theta);
            float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.02 * distance * distance);
            diffuse = max(dot(fragNormal, lightDir), 0.0) * intensity * attenuation * 3.0;
        }
    }
    
    vec3 light = vec3(ambient + diffuse);
    finalColor = texelColor * colDiffuse * vec4(light, 1.0);
}
)";

bool CheckMeshCollision(Vector3 oldPos, Vector3 newPos, Model& model, float radius) {
    Vector3 dir = { newPos.x - oldPos.x, newPos.y - oldPos.y, newPos.z - oldPos.z };
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if (dist < 0.0001f) return false;
    
    dir.x /= dist; dir.y /= dist; dir.z /= dist;
    
    Ray ray = { oldPos, dir };
    for (int i = 0; i < model.meshCount; i++) {
        RayCollision col = GetRayCollisionMesh(ray, model.meshes[i], model.transform);
        if (col.hit && col.distance < (radius + dist)) {
            return true;
        }
    }
    return false;
}

void start_backrooms() {
    InitWindow(1024, 768, "Backrooms - 3D GLB Model");
    SetTargetFPS(60);
    DisableCursor();

    // Load the external model file relative to the build directory
    Model model = LoadModel("../backrooms/backrooms_with_baked_textures.glb");
    
    Shader shader = LoadShaderFromMemory(vsCode, fsCode);
    int viewPosLoc = GetShaderLocation(shader, "viewPos");
    int flashlightOnLoc = GetShaderLocation(shader, "flashlightOn");
    int spotlightDirLoc = GetShaderLocation(shader, "spotlightDir");

    // Assign the shader to all materials in the model
    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].shader = shader;
    }

    Camera3D camera = { 0 };
    // Adjust starting position as necessary depending on where the model spawns
    camera.position = (Vector3){ 1.0f, 0.4f, 1.0f };
    camera.target = (Vector3){ 1.0f, 0.4f, 2.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float yaw = 0.0f;
    float pitch = 0.0f;
    Vector2 vel = {0, 0};
    float bobTime = 0.0f;
    int flashlightOn = 1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F)) flashlightOn = !flashlightOn;
        if (IsKeyPressed(KEY_ESCAPE)) break;

        Vector2 mouseDelta = GetMouseDelta();
        yaw -= mouseDelta.x * 0.003f;
        pitch -= mouseDelta.y * 0.003f;
        
        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;

        Vector3 forward = { cosf(pitch)*sinf(yaw), sinf(pitch), cosf(pitch)*cosf(yaw) };
        Vector3 flatForward = { sinf(yaw), 0.0f, cosf(yaw) };
        Vector3 flatRight = { cosf(yaw), 0.0f, -sinf(yaw) };

        Vector2 moveDir = {0, 0};
        if (IsKeyDown(KEY_W)) { moveDir.x += flatForward.x; moveDir.y += flatForward.z; }
        if (IsKeyDown(KEY_S)) { moveDir.x -= flatForward.x; moveDir.y -= flatForward.z; }
        if (IsKeyDown(KEY_A)) { moveDir.x += flatRight.x; moveDir.y += flatRight.z; } // Flipped A and D
        if (IsKeyDown(KEY_D)) { moveDir.x -= flatRight.x; moveDir.y -= flatRight.z; }

        if (moveDir.x != 0.0f || moveDir.y != 0.0f) {
            float len = sqrt(moveDir.x*moveDir.x + moveDir.y*moveDir.y);
            moveDir.x /= len; moveDir.y /= len;
        }

        vel.x += moveDir.x * 0.015f;
        vel.y += moveDir.y * 0.015f;
        vel.x *= 0.85f; // Friction
        vel.y *= 0.85f; // Friction

        float playerRadius = 0.4f; // A decent radius for stopping before hitting walls
        
        Vector3 nextX = { camera.position.x + vel.x, camera.position.y, camera.position.z };
        if (!CheckMeshCollision(camera.position, nextX, model, playerRadius)) {
            camera.position.x = nextX.x;
        } else {
            vel.x = 0;
        }

        Vector3 nextZ = { camera.position.x, camera.position.y, camera.position.z + vel.y };
        if (!CheckMeshCollision(camera.position, nextZ, model, playerRadius)) {
            camera.position.z = nextZ.z;
        } else {
            vel.y = 0;
        }

        float speed = sqrtf(vel.x*vel.x + vel.y*vel.y);
        if (speed > 0.01f) {
            bobTime += speed * 6.0f; // Lower frequency
        }
        camera.position.y = 0.4f + sinf(bobTime) * 0.04f; // Lower base height and amplitude
        camera.target = (Vector3){ camera.position.x + forward.x, camera.position.y + forward.y, camera.position.z + forward.z };

        // Update shader uniforms every frame
        SetShaderValue(shader, viewPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, flashlightOnLoc, &flashlightOn, SHADER_UNIFORM_INT);
        SetShaderValue(shader, spotlightDirLoc, &forward, SHADER_UNIFORM_VEC3);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                
                // Draw the GLB model using standard 3D logic
                DrawModel(model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

            EndMode3D();
            
            DrawText("BACKROOMS", 20, 20, 20, LIGHTGRAY);
            DrawText("F - Toggle Flashlight", 20, 50, 20, LIGHTGRAY);
            DrawText("ESC - Exit", 20, 80, 20, LIGHTGRAY);

            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 2, WHITE); // Crosshair
        EndDrawing();
    }
    
    EnableCursor();
    UnloadShader(shader);
    UnloadModel(model);
    CloseWindow();
}
