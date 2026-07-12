import os
import discord
import bpe_core
import sys

class MyClient(discord.Client):
    bpe: bpe_core.Bpe
    tok_file : str

    def __init__(self, tok_file: str, **kwargs):
        super().__init__(**kwargs)
        self.bpe = bpe_core.Bpe("")
        self.tok_file = tok_file
        
    async def on_ready(self):
        print(f'Logged on as {self.user}!')
        self.bpe.load_from_file(self.tok_file)

    async def on_message(self, message):
        if message.author.bot:
            return

        if not message.content:
            return

        if message.content == "gimme":
            await message.reply(self.bpe.fetch())
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
    
def main(args: list[str]):
    intents = discord.Intents.default()
    intents.message_content = True
    print(args)
    if len(args) <= 1:
        bpe_core.eprint("Usage: %s <file>" % args[0])
        return
    tok_file = args[1]
    client = MyClient(tok_file=tok_file, intents=intents)
    token = os.environ.get('API_KEY')
    if token is None:
        bpe_core.eprint("Source env.sh")
        return
    client.run(token)

if __name__ == "__main__":
    args = sys.argv
    main(args)
