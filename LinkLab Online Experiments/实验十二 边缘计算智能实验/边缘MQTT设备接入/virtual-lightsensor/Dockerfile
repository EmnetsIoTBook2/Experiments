FROM python:3.6-slim-buster
RUN pip install paho-mqtt -i http://mirrors.aliyun.com/pypi/simple/ --trusted-host mirrors.aliyun.com
ENV PYTHONUNBUFFERED=0
WORKDIR /app
COPY code/* ./
CMD ["python","app.py"]