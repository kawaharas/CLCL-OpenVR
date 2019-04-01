# How to build VFIVE using CLCL

**VFIVE** is an interactive visualization software for CAVE-type VR systems. 
It was develeped by Akira Kageyama (Kobe University) and Nobuaki Ohno (University of Hyogo). 
For details of VFIVE, please see the following paper;

&nbsp; *A. Kageyama, Y. Tamura, T. Sato:*  
&nbsp; *Visualization of Vector Field by Virtual Reality,*  
&nbsp; *Progress of Theoretical Physics Supplement, 138 (2000), 665-673.*


## Download source code

&nbsp; Download the source code of VFIVE and a sample dataset from the following URL:

&nbsp; &nbsp; [https://www.jamstec.go.jp/ceist/aeird/avcrg/vfive.ja.html](https://www.jamstec.go.jp/ceist/aeird/avcrg/vfive.ja.html)

&nbsp; &nbsp; vfive3.72Amt.zip  
&nbsp; &nbsp; tex_maker.zip *  
&nbsp; &nbsp; sample_little.tar.gz

&nbsp; \*  **tex_maker** is a program to generate texture images for menu panels of VFIVE. GLUT is required to compile it.

## Create project

&nbsp; Create new project on Visual Studio and add source code of VFIVE (\*.h and \*.cpp) to the project.

## Project settings

- Add ***&quot;&#123;&#36;PATH_TO_CLCL&#125;/include&quot;*** to 
&#91;C/C++&#93;-&#91;General&#93;-&#91;Additional Include Directories&#93;. *
- Add ***&quot;&#123;&#36;PATH_TO_CLCL&#125;/lib/x64&quot;*** to 
&#91;Linker&#93;-&#91;General&#93;-&#91;Additional Library Directories&#93;. *
- Add ***&quot;WIN32&quot;*** and ***&quot;&#095;CRT&#095;SECURE&#095;NO&#095;WARNINGS&quot;*** to 
&#91;C/C++&#93;-&#91;Preprocessor&#93;-&#91;Preprocessor Difinitions&#93;.
- Add &quot;***CLCL.lib***&quot; or &quot;***CLCL_openvr.lib***&quot; to 
&#91;Linker&#93;-&#91;Input&#93;-&#91;Additional Dependencies&#93;.
- Select ***&quot;Multi-threaded &#040;&frasl;MT&#041;&quot;*** in 
&#91;C/C++&#93;-&#91;Code Generation&#93;-&#91;Runtime Library&#93;.
- Select ***&quot;No&quot;*** in 
&#91;C/C++&#93;-&#91;Language&#93;-&#91;Conformance mode&#93;.

&nbsp; &nbsp; \*  Please set the appropriate directory for ***PATH_TO_CLCL***.  

## Source code modifications

&nbsp; **vfive.h**

&nbsp; For resolving a conflict between glext.h and GLEW.
```diff
  12 #ifdef WIN32
+    #ifndef USE_CLCL
  13 #include <GL/glext.h>
+    #endif
  14 #endif
```

&nbsp; Sample dataset is double precision.
```diff
- 56 typedef float ffloat_;     
- 56 //typedef double ffloat_;  
+ 56 //typedef float ffloat_;     
+ 57 typedef double ffloat_;  
```

&nbsp; **volren.cpp**

&nbsp; For resolving conflicts between glext.h and GLEW.
```diff
  11 #ifdef WIN32
+    #ifndef USE_CLCL
  12 PFNGLTEXIMAGE3DPROC glTexImage3D;
+    #endif
  13 #endif
```

```diff
  264 #ifdef WIN32
+     #ifndef USE_CLCL
  265   glTexImage3D =
  266     (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
+     #endif
  267 #endif
```

&nbsp; **panel.cpp**
```diff
  331 #ifdef WIN32
- 332   sprintf(command,"tex_maker %s 0x%x 0x%x %s",
- 333     label_temp,bgc,fgc,file_name);
+ 332   sprintf(command,"%s\\tex_maker %s 0x%x 0x%x %s",
+ 333     dir, label_temp,bgc,fgc,file_name);
  334 #else
```

&nbsp; **main.cpp** (Oculus SDK version only)

&nbsp; In Oculus SDK, left-eye image and right-eye image are drawn on same render buffer.
```diff
+     #ifndef USE_CLCL_OCULUS_SDK
  337     glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
+     #endif
```

## Execution

&nbsp; Rocate the extracted sample data (**sample_little.tar.gz**) and **tex_maker.exe** in the same directory as **vfive.exe**.  

&nbsp; &nbsp; &nbsp; +-- dynamo.v5  
&nbsp; &nbsp; &nbsp; +-- dynamo.x  
&nbsp; &nbsp; &nbsp; +-- dynamo.y  
&nbsp; &nbsp; &nbsp; +-- dynamo.z  
&nbsp; &nbsp; &nbsp; +-- dynamo.vel.x  
&nbsp; &nbsp; &nbsp; +-- dynamo.vel.y  
&nbsp; &nbsp; &nbsp; +-- dynamo.vel.z  
&nbsp; &nbsp; &nbsp; +-- dynamo.vor_z_sq  
&nbsp; &nbsp; &nbsp; +-- dynamo.vorticity_z  
&nbsp; &nbsp; &nbsp; +-- dynamo.temprature  
&nbsp; &nbsp; &nbsp; +-- dynamolines  
&nbsp; &nbsp; &nbsp; +-- dynamolines.x  
&nbsp; &nbsp; &nbsp; +-- dynamolines.y  
&nbsp; &nbsp; &nbsp; +-- dynamolines.z  
&nbsp; &nbsp; &nbsp; +-- dynamolines.c  
&nbsp; &nbsp; &nbsp; +-- **tex_maker.exe**  
&nbsp; &nbsp; &nbsp; +-- **vfive.exe**  
&nbsp; &nbsp; &nbsp; +-- ***openvr_api.dll*** (OpenVR version only)  

&nbsp; To launch VFIVE, type following command.  
&nbsp; \*  In the OpenVR version, "openvr_api.dll" needs to be placed in the same directory.  

```
  > vfive.exe dynamo.v5 -l dynamolines
```
