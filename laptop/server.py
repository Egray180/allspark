import asyncio
import websockets

connected_clients = set()

async def handler(websocket):
    print("New client connected.")
    connected_clients.add(websocket)
    try:
        async for message in websocket:
            print(f"Received: {message}")
            # (Optional) You could broadcast to other clients here if you want
    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected.")
    finally:
        connected_clients.remove(websocket)

async def main():
    server = await websockets.serve(handler, "0.0.0.0", 8765)
    print("WebSocket server started on port 8765")
    await server.wait_closed()

if __name__ == "__main__":
    asyncio.run(main())
