/*******************************************************************************************
*
*   raylib [texture] example - Import and display of Tiled map editor map
*
*   This example has been created using raylib 2.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include <stdlib.h>

#include "raylib.h"
#include "tmx.h"


// Helper function for the TMX lib to load a texture from a file
Texture2D *LoadMapTexture(const char *fileName);

// Helper function for the TMX lib to unload a texture that was previously loaded
void UnloadMapTexture(Texture2D *tex);

// Read a Tile map editor TMX map file and render the map into RenderTexture2D. 
// This is the main part of this example.
// This must be called after InitWindow().
void RenderTmxMapToFramebuf(const char *mapFIleName, RenderTexture2D *buf);

// Frame buffer into which the map is rendered
RenderTexture2D mapFrameBuffer;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");
    SetTargetFPS(60);

    // Create a frame buffer
    mapFrameBuffer = LoadRenderTexture(screenWidth, screenHeight);
    // Load Tiled TMX map and render it to the frame buffer
    RenderTmxMapToFramebuf("resources/map.tmx", &mapFrameBuffer);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // Flip along the y axis because OpenGL origin is at bottom left corner while Raylib is top left
            DrawTextureRec(
                mapFrameBuffer.texture,                  
                (Rectangle){0, 0, mapFrameBuffer.texture.width, -mapFrameBuffer.texture.height},
                (Vector2){0.0, 0.0},
                WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

Texture2D *LoadMapTexture(const char *fileName)
{
    Texture2D *tex = (Texture2D *)malloc(sizeof(Texture2D));
    if (tex != NULL) 
    {
        *tex = LoadTexture(fileName);
        TraceLog(LOG_INFO, "TMX texture loaded from %s", fileName);
        return tex;
    }
    return NULL;
}

void UnloadMapTexture(Texture2D *tex)
{
    if (tex != NULL) 
    {
        UnloadTexture(*tex);
        free(tex);
    }
}

void DrawTmxLayer(tmx_map *map, tmx_layer *layer)
{
    unsigned long row, col;
    unsigned int gid;
    unsigned int flip;
    tmx_tile *tile;
    unsigned int tile_width;
    unsigned int tile_height;
    Rectangle sourceRect;
    Rectangle destRect;
    Texture2D *tsTexture; // tileset texture

    for (row = 0; row < map->height; row++)
    {
        for (col = 0; col < map->width; col++)
        {
            gid = layer->content.gids[(row * map->width) + col];
            flip = gid & ~TMX_FLIP_BITS_REMOVAL;    // get flip operations from GID
            gid = gid & TMX_FLIP_BITS_REMOVAL;      // remove flip operations from GID to get tile ID
            tile = map->tiles[gid];
            if (tile != NULL)
            {
                // Get the tileset texture that defines this tile's texture
                if (tile->image != NULL)
                {
                    tsTexture = (Texture2D *)tile->image->resource_image;
                    tile_width = tile->image->width;
                    tile_height = tile->image->height;
                }
                else
                {
                    tsTexture = (Texture2D *)tile->tileset->image->resource_image;
                    tile_width = tile->tileset->tile_width;
                    tile_height = tile->tileset->tile_height;
                }

                sourceRect.x = tile->ul_x;
                sourceRect.y = tile->ul_y;
                sourceRect.width = destRect.width = tile_width;
                sourceRect.height = destRect.height = tile_height;
                destRect.x = col * tile_width;
                destRect.y = row * tile_height;

                // Deal with flips. From TMX file format documentation:
                // "When rendering a tile, the order of operation matters. The diagonal flip
                // (x/y axis swap) is done first, followed by the horizontal and vertical flips."
                if (flip & TMX_FLIPPED_DIAGONALLY)                
                {
                    TraceLog(LOG_INFO, "TODO: Flip GID %d diagonally", gid);
                    // TODO: How? GetTextureData to get Image and then work on the image?
                }
                if (flip & TMX_FLIPPED_HORIZONTALLY)
                {
                    sourceRect.height = -1 * sourceRect.height;
                }
                if (flip & TMX_FLIPPED_VERTICALLY)
                {
                    sourceRect.width = -1 * sourceRect.width;
                }

                if (row == 0 && col == 8)
                {
                    TraceLog(LOG_INFO, "GID %d, flip 0x%x, src (%f, %f, %f, %f), dst (%f, %f, %f, %f)",
                    gid, flip, sourceRect.x, sourceRect.y, sourceRect.width, sourceRect.height,
                    destRect.x, destRect.y, destRect.width, destRect.height);
                }
                // TODO: Take layer opacity into account
                DrawTexturePro(*tsTexture, sourceRect, destRect, (Vector2){0.0, 0.0}, 0.0, WHITE);
            }
        }
    }
}

void RenderTmxMapToFramebuf(const char *mapFIleName, RenderTexture2D *buf)
{
    tmx_layer *layer = NULL;

    // Setting these two function pointers allows TMX lib to load the tileset graphics and
    // set each tile's resource_image property properly.
    tmx_img_load_func = (void *(*)(const char *))LoadMapTexture;
    tmx_img_free_func = (void (*)(void *))UnloadMapTexture;
    tmx_map *mapTmx = tmx_load(mapFIleName);
    if (mapTmx == NULL) {
        tmx_perror("tmx_load");
        return;
    }

    BeginTextureMode(*buf); // start rendering into the buffer
        ClearBackground(SKYBLUE);
        // Iterate through TMX layers rendering them into buf
        layer = mapTmx->ly_head;
        while(layer)
        {
            if (layer->visible)
            {
                switch(layer->type)
                {
                    case L_LAYER:
                        TraceLog(LOG_INFO, "Render TMX layer \"%s\"", layer->name);
                        DrawTmxLayer(mapTmx, layer);
                        break;

                    // Group, Object and Image layer types are not implemented in this example
                    case L_GROUP:   // deliberate fall-through
                    case L_OBJGR:
                    case L_IMAGE:
                    case L_NONE:
                    default:
                        break;
                }
            }
            layer = layer->next;
        }
    EndTextureMode();   // stop rendering into the buffer

    tmx_map_free(mapTmx);
}
