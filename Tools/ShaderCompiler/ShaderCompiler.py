
import sys
import os

sDx11CommonFile = "/ShaderInclude/DX11/Common.hlsli"
sDx11VertexCommonFile = "/ShaderInclude/DX11/VertexCommon.hlsli"
sDx11PixelCommonFile = "/ShaderInclude/DX11/PixelCommon.hlsli"

sVkCommonFile = "/ShaderInclude/Vulkan/Common.glsl"
sVkVertexCommonFile = "/ShaderInclude/Vulkan/VertexCommon.glsl"
sVkPixelCommonFile = "/ShaderInclude/Vulkan/PixelCommon.glsl"

def main():

	iNumArgs = len(sys.argv)  

	if iNumArgs < 3:
		return
	
	sAPI = str(sys.argv[2])

	sScriptDir = os.path.dirname(__file__)
	sCommonFile = sDx11CommonFile if sAPI == "dx11" else sVkCommonFile
	sCommon = open(sScriptDir + sCommonFile).read()    	 		

	if sAPI != "dx11" and sAPI != "vk":
		print("Second parameter must be \"dx11\" or \"vk\"")
		return
	
	print("Target API: " + sAPI)
	
	sType = str(sys.argv[1])		

	if sType == "v":
		print("Building Vertex Shaders")
		sVCommonFile = sDx11VertexCommonFile if sAPI == "dx11" else sVkVertexCommonFile
		sCommon = sCommon + open(sScriptDir + sVCommonFile).read()
	elif sType == "p":
		print("Building Pixel Shaders")
		sVCommonFile = sDx11PixelCommonFile if sAPI == "dx11" else sVkPixelCommonFile
		sCommon = sCommon + open(sScriptDir + sVCommonFile).read()
	else:
		print("First parameter must be \"p\" or \"v\" !!! \n It's: " + sType)
		return
	
	sOutFolder = "DX11" if sAPI == "dx11"  else "VK"
	sBinExt = ".cso" if sAPI == "dx11"  else ".spv"	
	sShaderExt = ".hslsl"  if sAPI == "dx11"  else ".glsl"
		
	for i in range(3, iNumArgs):
		sFilename = str(sys.argv[i])		
		sName = os.path.splitext(os.path.basename(sFilename))[0]
		sDir = os.path.dirname(sFilename)
		sOutDir = sDir + "\\" + sOutFolder		
		if not os.path.exists(sOutDir):
			os.makedirs(sOutDir)
		sOutFilename = sOutDir  + "\\" + sName + sShaderExt		
		with open(sFilename) as oShaderFile:
			sShader = sCommon + oShaderFile.read()			
			with open(sOutFilename, mode='w', encoding='utf-8') as oOutFile:
				oOutFile.write(sShader)			
		sCompilerOutputFilename = os.path.splitext(sFilename)[0] + sBinExt		
		if sAPI == "dx11":
			sCompilerTypeFlag = "vs_5_0" if sType == "v" else "ps_5_0"
			os.system("fxc -E main -T " + sCompilerTypeFlag + " -Fo " + sCompilerOutputFilename + " " + sOutFilename)
		else:
			os.system(os.environ["VULKAN_SDK"] + "/Bin/glslc.exe " + sOutFilename + " -o " + sCompilerOutputFilename)
		os.remove(sOutFilename)

if __name__=="__main__": 
	main() 
