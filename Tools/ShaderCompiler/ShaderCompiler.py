
import sys
import os
import shutil

sDx11CommonFile = "/ShaderInclude/DX11/Common.hlsli"
sDx11VertexCommonFile = "/ShaderInclude/DX11/VertexCommon.hlsli"
sDx11GeomCommonFile = "/ShaderInclude/DX11/GeomCommon.hlsli"
sDx11PixelCommonFile = "/ShaderInclude/DX11/PixelCommon.hlsli"

sVkCommonFile = "/ShaderInclude/Vulkan/Common.glsl"
sVkVertexCommonFile = "/ShaderInclude/Vulkan/VertexCommon.glsl"
sVkGeomCommonFile = "/ShaderInclude/Vulkan/GeomCommon.glsl"
sVkPixelCommonFile = "/ShaderInclude/Vulkan/PixelCommon.glsl"

sLightingFile = "/ShaderInclude/Lighting.hlsli"

sPbrFile = "/ShaderInclude/PBR.hlsli"

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
		
	sType = str(sys.argv[1])		

	if sType == "v":
		sVCommonFile = sDx11VertexCommonFile if sAPI == "dx11" else sVkVertexCommonFile
		sCommon = sCommon + open(sScriptDir + sVCommonFile).read()
	elif sType == "p":
		sPCommonFile = sDx11PixelCommonFile if sAPI == "dx11" else sVkPixelCommonFile
		sCommon = sCommon + open(sScriptDir + sPCommonFile).read()
		sCommon = sCommon + open(sScriptDir + sLightingFile).read()
		sCommon = sCommon + open(sScriptDir + sPbrFile).read()
	elif sType == "g":
		sGCommonFile = sDx11GeomCommonFile if sAPI == "dx11" else sVkGeomCommonFile
		sCommon = sCommon + open(sScriptDir + sGCommonFile).read()
	else:
		print("First parameter must be \"p\" , \"g\" or \"v\" !!! \n It's: " + sType)
		return	
	
	sOutFolder = "DX11" if sAPI == "dx11"  else "VK"
	sBinExt = ".cso" if sAPI == "dx11"  else ".spv"	
	sSrcExt = ".hlsl"  if sAPI == "dx11"  else ".glsl"
		
	for i in range(3, iNumArgs):
		sFilename = str(sys.argv[i])		
		sName = os.path.splitext(os.path.basename(sFilename))[0]
		sDir = os.path.dirname(sFilename)
		sBinOutDir = sDir + "\\" + sOutFolder + "\\"
		sSrcOutDir = sBinOutDir + "\\Src\\"
		if not os.path.exists(sBinOutDir):
			os.makedirs(sBinOutDir)
		if not os.path.exists(sSrcOutDir):
			os.makedirs(sSrcOutDir)
		sSrcFilename = sSrcOutDir + sName + sSrcExt		
		sBinFilename = sBinOutDir + sName + sBinExt		

		with open(sFilename) as oShaderFile:
			sShader = oShaderFile.read()	

			if sAPI == "dx11":				
				sNewShader = ""
				for line in sShader.split("\n"):
					if "Texture(" in line:
						iCommaIdx = line.rfind(',')
						iBind = int(line[iCommaIdx+1 : iCommaIdx+2])						
						iSet = int(line[iCommaIdx-1 : iCommaIdx])
						line = line[:iCommaIdx+1] + str(iBind + iSet*4) + line[iCommaIdx+2:]
					sNewShader += line + '\n'
				sShader = sNewShader
			sShader = sCommon + sShader

			with open(sSrcFilename, mode='w', encoding='utf-8') as oOutFile:
				oOutFile.write(sShader)		

		#sCompilerOutputFilename = os.path.splitext(sFilename)[0] + sBinExt		
		
		if sAPI == "dx11":
			sCompilerTypeFlag = ""
			if sType == "v":
				sCompilerTypeFlag = "vs_5_0" 
			elif sType == "g":
				sCompilerTypeFlag  = "gs_5_0"
			elif sType == "p":
				sCompilerTypeFlag  = "ps_5_0"
			os.system("fxc -E main -T " + sCompilerTypeFlag + " -Fo " + sBinFilename + " -Od -Zi " + sSrcFilename)
		else:
			os.system(os.environ["VULKAN_SDK"] + "/Bin/glslc.exe -g " + sSrcFilename + " -o " + sBinFilename)

		# if os.path.exists(sOutDir):
		# 	shutil.rmtree(sOutDir)		

if __name__=="__main__": 
	main() 
