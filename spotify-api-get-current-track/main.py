from pprint import pprint
import requests

# put in environment var in future
SPOTIFY_ACCESS_TOKEN = ''
SPOTIFY_GET_CURRENT_TRACK_URL = 'https://api.spotify.com/v1/me/player/currently-playing'
client_id = 'e9d84e6801064176925e1b25a6ff45a7'
client_secret = 'c3ec526255ae4643b44703340d7c4f9c'
redirect_uri = 'http://localhost:8888/callback'
scope = 'user-read-playback-state user-modify-playback-state'


def get_current_track_info(access_token):
    response = requests.get(
        SPOTIFY_GET_CURRENT_TRACK_URL,
        headers={
            'Authorization': f'Bearer {access_token}'
        }
    )
    # gives json response -> dictionary format
    resp_json = response.json()  

    song_id = resp_json['item']['id']
    song_name = resp_json['item']['name']
    artists = resp_json['item']['artists']
    artist_names = ', '.join(
        [artist['name'] for artist in artists]
    )

    current_track_info = {
        "id": song_id,
        "name": song_name,
        "artists": artist_names
    }

    return current_track_info

def main():
    current_track_info = get_current_track_info(
        SPOTIFY_ACCESS_TOKEN
    )

    # nicer way to print dictionaries
    pprint(current_track_info, indent=4)

    # {
    #     "id": "123456",
    #     "name": "Song Name",
    #     "artists": "Artist Name",
    # }

if __name__ == "__main__":
    main()
