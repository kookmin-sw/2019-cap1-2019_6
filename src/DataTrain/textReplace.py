import os

# listdir : 특정 경로에 존재하는 파일과 디렉터리 목록을 구하는 함수 
dir1 = os.listdir('/Users/kimsangmin/Downloads/stairs_dataset/count/')
# chdir : 현재 작업 디렉터리를 변경하는 함수
os.chdir('/Users/kimsangmin/Downloads/stairs_dataset/count/')

for f in dir1:
    open_file = open(f,'r')
    line = open_file.readline()
    
    replaceText = line.replace("0", "1", 1) # 찾을 값, 바꿀 값, 바꿀 횟수
    # 예) 0, 0.xxx, 0.xxx, 0.xxx, 0.xxx => 1, 0.xxx, 0.xxx, 0.xxx, 0.xxx
    
    write_file = open(f,'w')
    write_file.write(replaceText)