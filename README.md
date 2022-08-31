由于 qvr 本身的问题，导致 Launcher 切到其他应用再切回来时，qvr 状态会出现错误，以至于 6Dof 无法恢复。  
  
此 app 的作用是，在启动后率先持有 qvr，使其他应用不会让 qvr 进入 stop 状态。  
  
### Build  
正常使用 Android Studio 的 make 来进行构建。  
生成的文件: app/build/intermediates/cmake/debug/obj/arm64-v8a/qvrholder  

### 安装  
adb push ${QvrHolderPath}/qvrholder /system/bin/qvrholder  

### 运行  
#### 直接运行  
adb shell qvrholder  
#### 后台运行  
adb shell qvrhodler &
