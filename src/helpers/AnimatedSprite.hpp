#pragma once

#include <raylib.h>

/*
    This only works with horizontal sprite sheets; see the Update() function for logic
    that only considers moving the right along the x axis
*/

class AnimatedSprite {
public:
    AnimatedSprite(const char* spriteFile, int fw, int fh, int cf, float fps)
        : mFPS(fps),
          mCurrentFrame(0),
          mFrameCount(cf),
          mFrameHeight(fh),
          mFrameWidth(fw),
          mFrameTimer(0.) {

        mTexture = LoadTexture(spriteFile);
        mSourceRect = { 0, 0, static_cast<float>(fw), static_cast<float>(fh) };
    } 

    ~AnimatedSprite() {
        UnloadTexture(mTexture);
    }

    void draw(Vector2 position) const {
        DrawTextureRec(mTexture, mSourceRect, position, WHITE);
    }

    void changeTexture(const char* newSpriteFile) {
        UnloadTexture(mTexture);
        mTexture = LoadTexture(newSpriteFile);
    }

    void Update() {
        mFrameTimer += GetFrameTime();
        if (mFrameTimer >= 1.f / mFPS) {
            mFrameTimer = 0.f;
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSourceRect.x = static_cast<float>(mCurrentFrame * mFrameWidth); 
        }
    }

private:
    float mFPS;
    float mFrameTimer;
    int mCurrentFrame;
    int mFrameCount;
    int mFrameHeight;
    int mFrameWidth;

    Rectangle mSourceRect;
    Texture2D mTexture;

};