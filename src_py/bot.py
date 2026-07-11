import os
import discord
import bpe_core

class MyClient(discord.Client):
    bpe: bpe_core.Bpe

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.bpe = bpe_core.Bpe("")
        
    async def on_ready(self):
        print(f'Logged on as {self.user}!')
        self.bpe.load_from_file("outputs/big_data.txt")

    async def on_message(self, message):
        if not message.content:
            return

        if message.content == "gimme":
            await message.channel.send(self.bpe.fetch())
            return

        if message.content[0] != '!':
            return

        message.content = message.content[1:]

        if not message.content:
            await message.channel.send("mb bot.py failed")
            return

        ret = self.bpe.follows(message.content)

        if not ret:
            await message.channel.send("mb bot.py failed")
            return

        await message.channel.send(ret)

intents = discord.Intents.default()
intents.message_content = True

client = MyClient(intents=intents)
token = os.environ.get('API_KEY')
client.run(token)
