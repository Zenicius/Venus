#pragma once

#include <string>

#include "Engine/Base.h"
#include "Assets/Asset.h"

namespace Venus {

	enum class TextureFilterMode 
	{
		Point = 0,
		Bilinear = 1
	};

	enum class TextureWrapMode
	{
		Repeat = 0,
		Mirrored = 1,
		ClampToEdge = 2,
		ClampToBorder = 3
	};

	enum class TextureFormat
	{
		SRGB = 0,
		RGBA8 = 1,
		RGBA16F = 2,
		RGBA32F = 3
	};

	enum class TextureType
	{
		Texture2D = 0,
		TextureCube = 1
	};

	struct TextureProperties
	{
		// Defaults
		TextureFilterMode Filter = TextureFilterMode::Bilinear;
		TextureWrapMode WrapMode = TextureWrapMode::Repeat;
		TextureFormat Format = TextureFormat::RGBA8;
		bool FlipVertically = true;
		bool GenerateMipmaps = true;
		bool UseMipmaps = false;

		bool operator==(const TextureProperties& other)
		{
			bool isEqual = Filter == other.Filter && WrapMode == other.WrapMode && Format == other.Format
							&& FlipVertically == other.FlipVertically && GenerateMipmaps == other.GenerateMipmaps 
							&& UseMipmaps == other.UseMipmaps;

			return isEqual;
		}

		bool operator!=(const TextureProperties& other)
		{
			return !operator==(other);
		}
	};

	class Texture : public Asset
	{
		public:
			virtual ~Texture() = default;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
			virtual uint32_t GetRendererID() const = 0;

			virtual std::string GetPath() const = 0;

			virtual void Reload() = 0;
			virtual void SetData(void* data, uint32_t size, uint32_t mipLevel = 0) = 0;
			virtual void GenerateMips() = 0;

			virtual void Bind(uint32_t slot = 0) const = 0;

			virtual bool IsLoaded() const = 0;
			virtual TextureProperties GetProperties() = 0;
			virtual void SetProperties(TextureProperties props, bool reload) = 0;
			virtual TextureType GetType() const = 0;
			virtual uint32_t GetMipLevelCount() const = 0;
			virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const = 0;

			virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
		public:
			static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureProperties props = TextureProperties());
			static Ref<Texture2D> Create(const std::string& path, TextureProperties props = TextureProperties());
	};

	class TextureCube : public Texture
	{
		public:
			static Ref<TextureCube> Create(uint32_t width, uint32_t height, TextureProperties props = TextureProperties());
			static Ref<TextureCube> Create(std::vector<std::string> paths, TextureProperties props = TextureProperties());
	};
}
