#pragma once

namespace Epoch
{
	enum class ShaderStage
	{
		Vertex,
		Pixel,
		Compute,
	};

	struct ShaderCompileInput
	{
		std::string source;

		std::string entryPoint = "main";
		ShaderStage stage;

		bool optimize = true;
		bool debug = false;
	};

	struct ShaderCompileOutput
	{
		ShaderStage stage;
		std::vector<uint8_t> bytecode;
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;
		~ShaderCompiler() = default;

		bool Compile(const ShaderCompileInput& aInput, ShaderCompileOutput& outOutput);

	private:

	};
}
