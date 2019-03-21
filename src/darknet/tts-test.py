from google.cloud import texttospeech
import os
os.environ["GOOGLE_APPLICATION_CREDENTIALS"]="BeEyes-8a3077c1a3bc.json"
import io
import pygame

def detect_text(path):
    """Detects text in the file."""
    from google.cloud import vision
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
    print("stringline : ", stringline)

    return stringline
        # print('bounds: {}'.format(','.join(vertices)))

def startFingerTTS():
    # Instantiates a client
    client = texttospeech.TextToSpeechClient()
    
    doc = detect_text('screenshot.jpg')
  
    # Set the text input to be synthesized
    synthesis_input = texttospeech.types.SynthesisInput(text=doc)
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
    
    fw = open('input.txt', 'w')

    if doc == "":
        fw.write("글자를 읽을 수 없습니다.")
    else:
        fw.write("글자를 모두 읽었습니다.")

    fw.close()

    startBollardTTS()

def startBollardTTS():
    # Instantiates a client
    client = texttospeech.TextToSpeechClient()
    linetmp = "start TTS Function"
    print(linetmp)
    # TTS 무한루프
    while True :
        # 위험 경고 텍스트 메세지 불러오기 
        f = open('input.txt', 'r')
        line = f.readline()
        # 객체를 인식하지 못하거나 똑같은 객체를 인식한 경우에는 아래 while문을 돌리지 않는다.
        # 처음에는 while문을 무조건 돌리고, 이전에 인식한 객체와 다를 경우에만 돌린다. 
        while line != linetmp and line != "":
            if line == 'cell phone':
                startFingerTTS()
                break
            else :
                # print("line : ", line)
                # Set the text input to be synthesized
                synthesis_input = texttospeech.types.SynthesisInput(text=line)
                print(type(synthesis_input))
                linetmp = line
                # print("linetmp : ", linetmp)
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
            

# TTS 기능 시작 
startBollardTTS()
       
