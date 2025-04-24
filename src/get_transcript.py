# save this as get_transcript.py
import sys
from youtube_transcript_api import YouTubeTranscriptApi

def get_transcript(video_url):
    # Extract video ID from URL
    video_id = video_url.split('v=')[-1]

    try:
        transcript = YouTubeTranscriptApi.get_transcript(video_id)
        return transcript
    except Exception as e:
        return str(e)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 get_transcript.py <YouTube video URL>")
        sys.exit(1)

    video_url = sys.argv[1]
    transcript = get_transcript(video_url)

    # Check if the transcript is a list (indicating successful retrieval)
    if isinstance(transcript, list):
        # Print the transcript to stdout, C++ will read this output
        for entry in transcript:
            print(f"{entry['start']}: {entry['text']}")
    else:
        # Print the error message to stdout
        print("Error retrieving transcript:", transcript)
