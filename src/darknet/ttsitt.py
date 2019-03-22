from google.cloud import texttospeech
from google.cloud import vision
import os
os.environ["GOOGLE_APPLICATION_CREDENTIALS"]="google-api key"
import io
import pygame

def TTSfunc(currentString):
    # Instantiates a client 
    client = texttospeech.TextToSpeechClient()
 
    synthesis_input = texttospeech.types.SynthesisInput(text=currentString)
    # Build the voice request, select the language code ("en-US") and the ssml
    # voice gender ("neutral")

    voice = texttospeech.types.VoiceSelectionParams(
        language_code='ko-KR',
        ssml_gender=texttospeech.enums.SsmlVoiceGender.NEUTRAL)

    # Select the type of audio file you want returned
    audio_config = texttospeech.types.AudioConfig(
        audio_encoding=texttospeech.enums.AudioEncoding.MP3)

    # Perform the text-to-speech request on the text input with the selected
    # voice parameters and audio file type
    response = client.synthesize_speech(synthesis_input, voice, audio_config)

    # The response's audio_content is binary.
    with open('output.mp3', 'wb') as out:
        # Write the response to the output file.
        out.write(response.audio_content)
        # print('Audio content written to file "output.mp3"')

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
        # print('\n"{}"'.format(text.description))
        stringline = stringline + "\n" + text.description
        # break
        # vertices = (['({},{})'.format(vertex.x, vertex.y)
        #             for vertex in text.bounding_poly.vertices])
    # print('bounds: {}'.format(','.join(vertices)))

    print("stringline : ", stringline)

    return stringline

def startFingerTTS():
    # Instantiates a client
    client = texttospeech.TextToSpeechClient()
    
    doc = detect_text('screenshot.jpg')

    fw = open('input.txt', 'w')
    if doc == "":
        fw.write("글자를 읽을 수 없습니다.")
    else :
        fw.write("글자를 모두 읽었습니다.")
    fw.close()

    # Set the text input to be synthesized
    TTSfunc(doc)

def startTTS():
    # Instantiates a client
    linetmp = "Init"
    print(linetmp)
    # TTS 무한루프
    while True :
        f = open('input.txt', 'r')
        line = f.readline()
        while line != linetmp and line != "":
            if line == 'cell phone':
                startFingerTTS()
                linetmp = "Init"
                break
            else :
                # Set the text input to be synthesized
                TTSfunc(line)
                linetmp = line

# TTS 기능 시작 
startTTS()

