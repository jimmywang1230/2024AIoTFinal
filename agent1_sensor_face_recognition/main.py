import cv2
import serial
import time
import bluetooth
from deepface import DeepFace


def check_faces(frame):
    memberDetection = 0

    try:
        results = DeepFace.find(
            frame,
            "./agent1_sensor_face_recognition/database",
            enforce_detection=False,
            silent=True,
            threshold=0.5,
        )

        for result in results:
            if not result.empty:
                print(f"{result.iloc[0]}\n")
                if result.iloc[0]["distance"] < 0.4:
                    identity = result.iloc[0]["identity"].split("\\")[-2]
                    print(f"=== detect {identity}")

                    if identity == "english":
                        memberDetection += 4
                    elif identity == "kp":
                        memberDetection += 2
                    else:
                        memberDetection += 1

                    # target_x, target_y, target_w, target_h = (
                    #     result["target_x"],
                    #     result["target_y"],
                    #     result["target_w"],
                    #     result["target_h"],
                    # )

                    # cv2.putText(
                    #     frame,
                    #     f"${result['identity']}",
                    #     (target_x, target_y),
                    #     cv2.FONT_HERSHEY_SIMPLEX,
                    #     1,
                    #     (255, 0, 0),
                    #     3,
                    # )

                    # cv2.rectangle(
                    #     frame,
                    #     (target_x, target_y),
                    #     (target_x + target_w, target_y + target_h),
                    #     (255, 0, 0),
                    #     2,
                    # )

    except ValueError:
        pass

    # filter out the unknown face
    if memberDetection != 0:
        print(f"memberDetection: {bytes([memberDetection])}")
        # serial.write(bytes(chr(memberDetection).encode()))
        socket.send(chr(memberDetection))
    return frame


# serial = serial.Serial("COM6", 9600)

# create bt socket and connect to the device
socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect(('98:D3:31:80:67:70', 1))


time.sleep(2)
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FPS, 30)
cv2.namedWindow("img", cv2.WINDOW_NORMAL | cv2.WINDOW_KEEPRATIO)
counter = 0

while True:
    ret, frame = cap.read()

    if ret:
        if counter % 30 == 0:
            counter = 0
            try:
                frame = check_faces(frame.copy())
            except ValueError:
                pass

        counter += 1
        cv2.imshow("img", frame)

    # Stop if escape key is pressed
    key = cv2.waitKey(30) & 0xFF
    if key == 27:
        break

cap.release()
socket.close()
