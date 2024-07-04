import whisper

model = whisper.load_model("base")
result = model.transcribe("../material/16k16bit.mp3")
print(result["text"])