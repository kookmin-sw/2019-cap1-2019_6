from google.cloud import texttospeech
from google.cloud import vision
import os
os.environ["GOOGLE_APPLICATION_CREDENTIALS"]="BeEyes-key_number.json"
import io
import pygame
import time

def TTSfunc(currentString):
    # Instantiates a client 
    client = texttospeech.TextToSpeechClient()
    synthesis_input = texttospeech.types.SynthesisInput(text=currentString)

    voice = texttospeech.types.VoiceSelectionParams(
        language_code='ko-KR',
        ssml_gender=texttospeech.enums.SsmlVoiceGender.NEUTRAL)

    audio_config = texttospeech.types.AudioConfig(
        audio_encoding=texttospeech.enums.AudioEncoding.MP3)

    # Perform the text-to-speech request on the text input with the selected
    # voice parameters and audio file type
    response = client.synthesize_speech(synthesis_input, voice, audio_config)

    # The response's audio_content is binary.
    with open('output.mp3', 'wb') as out:
        out.write(response.audio_content)
 
    # mp3 파일 바로 재생시키는 pygame 코드 
    mp3_file = 'output.mp3'

    freq = 22000
    bitsize = -16
    channels = 1
    buffers = 2048

    pygame.mixer.init(freq, bitsize, channels, buffers)
    pygame.mixer.music.load(mp3_file)
    pygame.mixer.music.play()

    clock = pygame.time.Clock()
    while pygame.mixer.music.get_busy():
        clock.tick(30)
    pygame.mixer.quit()

def detect_text(path):
    """Detects text in the file."""
    #from google.cloud import vision
    client = vision.ImageAnnotatorClient()

    with io.open(path, 'rb') as image_file:
        content = image_file.read()

    image = vision.types.Image(content=content)

    response = client.text_detection(image=image)
    texts = response.text_annotations
  
    stringline = ""
    
    for text in texts:
        stringline = stringline + "\n" + text.description

    print("stringline : ", stringline)

    return stringline

def startFingerTTS():
    # Instantiates a client
    fw = open('input.txt','w')
    client = texttospeech.TextToSpeechClient()
    doc = detect_text('screenshot.jpg')
    
    if doc == "":
        doc = "글자를 읽을 수 없습니다. "
        TTSfunc(doc)
    else:
        TTSfunc(doc)
        time.sleep(1)        
        cur = "글자를 모두 읽었습니다"
        TTSfunc(cur)
    
    return

def initTTS():
    fw = open('input.txt','w')
    fw.write("Init")
 

def startTTS():
    
    linetmp = "Init"
    TTSfunc("비아이즈 실행합니다.")
    while True :
        f = open('input.txt', 'r')
        line = f.readline()
        while linetmp != line and line != "":
            if line == "init":
                break
            if line == 'finger':
                startFingerTTS()
                initTTS()
                break
            else:
                print(line)
                TTSfunc(line)
                initTTS()
                break
# TTS 기능 시작 
startTTS()
