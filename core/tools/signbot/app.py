import os
import logging
from slack_bolt import App
from slack_bolt.adapter.socket_mode import SocketModeHandler
import touch


app = App(token=os.environ.get("SLACK_BOT_TOKEN"))


@app.event("message")
def handle_message(event, client, say):
    channel = event.get("channel")
    text = event.get("text")
    files = event.get("files")
    user = event.get("user")
    event_ts = event.get("event_ts")
    thread_ts = event.get("thread_ts")

    if user == 'U029WTSLB8W': # signbot user id
        return

    if thread_ts is None:
        return

    msgs = client.conversations_replies(channel=channel, ts=thread_ts)['messages']
    msgs = [m for m in msgs if m['user'] != 'U029WTSLB8W']
    touch.fetch_files_from_msgs(client, msgs)

    if text == '<@U029WTSLB8W>':
        touch.create_cosign(client, msgs)
        touch.process_cosign(client, msgs)
        touch.reply_cosign(client, channel, thread_ts)
        return

    if files is not None:
        touch.process_cosign(client, msgs)
        touch.reply_cosign(client, channel, thread_ts)
        return

if __name__ == "__main__":
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    logger.addHandler(logging.StreamHandler())
    SocketModeHandler(app, os.environ["SLACK_APP_TOKEN"]).start()
