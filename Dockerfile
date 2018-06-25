FROM python:2.7.13-jessie

RUN apt-get install libpq-dev

EXPOSE 9000

COPY requirements.txt /.deembox/xcommodule/requirements.txt
RUN pip install -r /.deembox/xcommodule/requirements.txt

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod -R +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]

RUN mkdir /src \
 && mkdir /src/modules

COPY ./run.py /src/run.py
COPY ./modules /src/modules

CMD [ "python", "/src/run.py" ]
