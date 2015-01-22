/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Gaff_Animation.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

Animation::Animation(const aiAnimation* animation):
	_animation(animation)
{
}

Animation::Animation(const Animation& animation):
	_animation(animation._animation)
{
}

Animation::Animation(void):
	_animation(nullptr)
{
}

Animation::~Animation(void)
{
}

const char* Animation::getName(void) const
{
	assert(_animation);
	return _animation->mName.C_Str();
}

double Animation::getDuration(void) const
{
	assert(_animation);
	return _animation->mDuration;
}

double Animation::getFramesPerSecond(void) const
{
	assert(_animation);
	return _animation->mTicksPerSecond;
}

unsigned int Animation::getNumNodeChannels(void) const
{
	assert(_animation);
	return _animation->mNumChannels;
}

unsigned int Animation::getNumMeshChannels(void) const
{
	assert(_animation);
	return _animation->mNumMeshChannels;
}

NodeAnimation Animation::getNodeAnimation(unsigned int channel) const
{
	assert(_animation && channel < _animation->mNumChannels);
	return NodeAnimation(_animation->mChannels[channel]);
}

MeshAnimation Animation::getMeshAnimation(unsigned int channel) const
{
	assert(_animation && channel < _animation->mNumMeshChannels);
	return MeshAnimation(_animation->mMeshChannels[channel]);
}

bool Animation::valid(void) const
{
	return _animation != nullptr;
}

const Animation& Animation::operator=(const Animation& rhs)
{
	_animation = rhs._animation;
	return *this;
}

bool Animation::operator==(const Animation& rhs) const
{
	return _animation == rhs._animation;
}

bool Animation::operator!=(const Animation& rhs) const
{
	return _animation != rhs._animation;
}

NS_END
