#pragma once
#include <iostream> // remove me later
#include <raylib.h>

/*
    This only works with horizontal sprite sheets; see the update() function for logic
    that only considers moving the right along the x axis
*/

class AnimatedSprite {
public:
    AnimatedSprite(Texture2D sprite, int fw, int fh, int fc, float fps)
        : mFPS(fps),
          mCurrentFrame(0),
          mFrameCount(fc),
          mFrameHeight(fh),
          mFrameWidth(fw),
          mFrameTimer(0.), 
          mFrameZero(0),
          mTexture(sprite),
          mUpdating(0) {
        if (mTexture.id == 0) {
            std::cout << "unable to load the sprite texture\n";
        }
        mSourceRect = { 0, 0, static_cast<float>(fw), static_cast<float>(fh) };
    } 

    ~AnimatedSprite() {
    }

    void draw(Vector2 position) const {
        auto halfX = mFrameWidth / 2;
        auto halfY = mFrameHeight / 2;
        DrawTextureRec(
            mTexture, 
            mSourceRect,
            {position.x - halfX, position.y - halfY},
            WHITE
        );
    }

    void setZeroFrame(int zf) { mFrameZero= zf; }

    void update() {
        ++mUpdating;
        if (mUpdating > 0) {
            mFrameTimer += GetFrameTime();
            if (mFrameTimer >= 1.f / mFPS) {
                mFrameTimer = 0.f;
                mCurrentFrame = (mCurrentFrame + 1) % mFrameCount + mFrameZero;
                mSourceRect.x = static_cast<float>(mCurrentFrame * mFrameWidth); 
            }
            --mUpdating;
        }
    }

private:
    float mFPS;
    float mFrameTimer;
    int mCurrentFrame;
    int mFrameCount;
    int mFrameHeight;
    int mFrameWidth;
    int mFrameZero;
    int mUpdating;

    Rectangle mSourceRect;
    Texture2D mTexture;

};