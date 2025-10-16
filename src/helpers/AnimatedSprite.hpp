#pragma once
#include <string>
#include <raylib.h>

/*
    This only works with horizontal sprite sheets; see the update() function for logic
    that only considers moving the right along the x axis
*/

class AnimatedSprite {
public:
    inline AnimatedSprite(std::string sp, int fw, int fh, int fc, float fps)
        : mFPS(fps),
          mCurrentFrame(0),
          mFrameCount(fc),
          mFrameHeight(fh),
          mFrameWidth(fw),
          mFrameTimer(0.f), 
          mFrameZero(0),
          mTexture(),
          mUpdating(0) {
        if (mTexture.id == 0) {
            mTexture = LoadTexture(sp.c_str());
        }
        mSourceRect = { 0, 0, static_cast<float>(fw), static_cast<float>(fh) };
    } 

    inline ~AnimatedSprite() { }

    inline void draw(Vector2 position) const {
        auto halfX = mFrameWidth / 2;
        auto halfY = mFrameHeight / 2;
        DrawTextureRec(
            mTexture, 
            mSourceRect,
            { position.x - halfX, position.y - halfY },
            WHITE
        );
    }

    inline bool onFinalFrame() {
        return mCurrentFrame == (mFrameZero + mFrameCount - 1);
    }

    inline void reset() {
        mCurrentFrame = mFrameZero;
        mFrameTimer = 0.f;
    }

    inline bool setTextureFile(std::string tf) {
        if (mTexture.id != 0) {
            UnloadTexture(mTexture);
        }
        mTexture = LoadTexture(tf.c_str());
        return (mTexture.id == 0) ? false : true;
    }

    inline void setZeroFrame(int zf) { mFrameZero = zf; }

    inline void update() {
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