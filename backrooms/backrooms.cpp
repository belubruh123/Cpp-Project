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
    float ambient = 0.025; // Brighter baseline
    float spotLightEffect = sin(fragPosition.x * 0.5) + cos(fragPosition.z * 0.5);
    float brightPart = smoothstep(1.0, 2.0, spotLightEffect) * 1.2; // Brighter peaks
    ambient += brightPart; // Add light blobs
    
    float diffuse = 0.0;
    if (flashlightOn == 1) {
        float theta = dot(lightDir, normalize(-spotlightDir));
        if (theta > 0.75) { // Cutoff angle for flashlight
            float intensity = smoothstep(0.75, 0.85, theta);
            float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.008 * distance * distance);
            diffuse = max(dot(fragNormal, lightDir), 0.0) * intensity * attenuation * 4.5;
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Backrooms");
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    InitAudioDevice();
    SetTargetFPS(60);
    DisableCursor();

    // Load the external model file relative to the build directory
    Model model = LoadModel("../backrooms/backrooms_with_baked_textures.glb");

    Music noise = LoadMusicStream("../backrooms/noise.wav");
    noise.looping = true;
    SetMusicVolume(noise, 5.0f); // Increased volume to make it audible
    PlayMusicStream(noise);
    
    Sound footstep = LoadSound("../backrooms/footstep.mp3");
    SetSoundVolume(footstep, 4.5f); // 10% lower than 5.0f
    
    Sound whisper = LoadSound("../backrooms/whisper.mp3");
    SetSoundVolume(whisper, 1.5f);
    float whisperTimer = GetRandomValue(30, 90);

    Music a4_music = LoadMusicStream("../backrooms/A4 - Childishly fresh eyes.mp3");
    SetMusicVolume(a4_music, 0.675f); // 25% lower than 0.9f
    PlayMusicStream(a4_music);
    PauseMusicStream(a4_music);
    int a4_state = 0; // 0 = waiting, 1 = playing
    float a4_timer = GetRandomValue(60, 120); // wait 1-2 minutes before playing

    Texture2D smiler1 = LoadTexture("../backrooms/smiler1.png");
    Texture2D smiler2 = LoadTexture("../backrooms/smiler2.png");
    Texture2D activeSmilerTex = smiler1;
    
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
    bool footstepPlayed = false;
    
    Vector3 smilerPos = { -1000.0f, 1.0f, -1000.0f };
    bool smilerActive = false;
    float smilerTimer = 0.0f;

    while (!WindowShouldClose()) {
        if (!IsMusicStreamPlaying(noise)) PlayMusicStream(noise);
        UpdateMusicStream(noise);
        UpdateMusicStream(a4_music);

        // Whisper logic
        whisperTimer -= GetFrameTime();
        if (whisperTimer <= 0.0f) {
            PlaySound(whisper);
            whisperTimer = GetRandomValue(30, 90);
        }

        // A4 Music Logic
        a4_timer -= GetFrameTime();
        if (a4_timer <= 0.0f) {
            if (a4_state == 0) {
                a4_state = 1;
                a4_timer = GetRandomValue(30, 67);
                ResumeMusicStream(a4_music);
            } else {
                a4_state = 0;
                a4_timer = GetRandomValue(60, 120);
                PauseMusicStream(a4_music);
            }
        }

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
            bobTime += speed * 4.0f; // Lower frequency
        }
        
        if (sinf(bobTime) < -0.9f) {
            if (!footstepPlayed) {
                PlaySound(footstep);
                footstepPlayed = true;
            }
        } else if (sinf(bobTime) > 0.0f) {
            footstepPlayed = false;
        }

        // Smiler logic
        if (!smilerActive) {
            for (int i = 0; i < 10; i++) {
                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float dist = GetRandomValue(7, 12); // Reverted back to 7-12
                Vector3 candidatePos = { camera.position.x + cosf(angle)*dist, 1.0f, camera.position.z + sinf(angle)*dist };
                
                float spotLightEffect = sinf(candidatePos.x * 0.5f) + cosf(candidatePos.z * 0.5f);
                if (spotLightEffect < 0.6f) { // Dark spot (loosened slightly)
                    if (!CheckMeshCollision(camera.position, candidatePos, model, 0.1f)) {
                        smilerPos = candidatePos;
                        smilerActive = true;
                        activeSmilerTex = GetRandomValue(0, 1) == 0 ? smiler1 : smiler2;
                        break;
                    }
                }
            }
        } else {
            float distToSmiler = Vector3Distance(camera.position, smilerPos);
            Vector3 dirToSmiler = Vector3Normalize(Vector3Subtract(smilerPos, camera.position));
            float theta = Vector3DotProduct(forward, dirToSmiler);
            
            if (distToSmiler < 3.0f) {
                smilerActive = false; // Too close
            } else if (flashlightOn == 1 && theta > 0.75f) {
                smilerActive = false; // Pointed flashlight at it
            }
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
                
                if (smilerActive) {
                    DrawBillboard(camera, activeSmilerTex, smilerPos, 2.0f, WHITE);
                }

            EndMode3D();
            
            DrawText("BACKROOMS", 20, 20, 20, LIGHTGRAY);
            DrawText("F - Toggle Flashlight", 20, 50, 20, LIGHTGRAY);
            DrawText("ESC - Exit", 20, 80, 20, LIGHTGRAY);

            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 2, WHITE); // Crosshair
        EndDrawing();
    }
    
    EnableCursor();
    UnloadTexture(smiler1);
    UnloadTexture(smiler2);
    UnloadSound(footstep);
    UnloadSound(whisper);
    UnloadMusicStream(a4_music);
    UnloadMusicStream(noise);
    CloseAudioDevice();
    UnloadShader(shader);
    UnloadModel(model);
    CloseWindow();
}
