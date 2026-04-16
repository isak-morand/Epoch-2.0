#pragma once

namespace Epoch
{
	enum class ShaderStage
	{
		None = 0,

		Vertex,
		Pixel,
		Compute,

		Fragment = Pixel,
	};

	struct ShaderCompileInput
	{
		std::string source;

		std::string entryPoint = "main";
		ShaderStage stage = ShaderStage::None;

		bool optimize = true;
		bool debug = false;
	};

	struct ShaderCompileOutput
	{
		ShaderStage stage = ShaderStage::None;
		std::vector<uint8_t> bytecode;
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;
		~ShaderCompiler() = default;

		bool Compile(const ShaderCompileInput& aInput, ShaderCompileOutput& outOutput);
	};
}
