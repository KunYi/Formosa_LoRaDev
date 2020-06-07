package main

import (
	"context"
	"crypto/tls"
	"flag"
	log "log"

	api "github.com/brocaar/chirpstack-api/go/as/external/api"
	"github.com/pkg/errors"
	grpc "google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/credentials"
)

// JWTCredentials provides JWT credentials for gRPC
type JWTCredentials struct {
	token string
}

// GetRequestMetadata returns the meta-data for a request.
func (j *JWTCredentials) GetRequestMetadata(ctx context.Context, url ...string) (map[string]string, error) {
	return map[string]string{
		"authorization": j.token,
	}, nil
}

// RequireTransportSecurity ...
func (j *JWTCredentials) RequireTransportSecurity() bool {
	return false
}

// SetToken sets the JWT token.
func (j *JWTCredentials) SetToken(token string) {
	j.token = token
}

var (
	username       string
	password       string
	file           string
	apiHost        string
	apiInsecure    bool
	jwtCredentials *JWTCredentials
)

func init() {
	jwtCredentials = &JWTCredentials{}

	flag.StringVar(&username, "username", "admin", "LoRa App Server username")
	flag.StringVar(&password, "password", "admin", "LoRa App Server password")
	flag.StringVar(&file, "file", "", "Path to Excel file")
	flag.StringVar(&apiHost, "api", "192.168.43.68:8080", "hostname:port to LoRa App Server API")
	flag.BoolVar(&apiInsecure, "api-insecure", true, "LoRa App Server API does not use TLS")
	flag.Parse()
}

func getGRPCConn() (*grpc.ClientConn, error) {
	dialOpts := []grpc.DialOption{
		grpc.WithBlock(),
		grpc.WithPerRPCCredentials(jwtCredentials),
	}

	if apiInsecure {
		log.Println("using insecure api")
		dialOpts = append(dialOpts, grpc.WithInsecure())
	} else {
		dialOpts = append(dialOpts, grpc.WithTransportCredentials(credentials.NewTLS(&tls.Config{
			InsecureSkipVerify: true,
		})))
	}

	conn, err := grpc.Dial(apiHost, dialOpts...)
	if err != nil {
		return nil, errors.Wrap(err, "grpc dial error")
	}

	return conn, nil
}

func login(conn *grpc.ClientConn) error {
	internalClient := api.NewInternalServiceClient(conn)

	resp, err := internalClient.Login(context.Background(), &api.LoginRequest{
		Username: username,
		Password: password,
	})
	if err != nil {
		return errors.Wrap(err, "login error")
	}

	jwtCredentials.SetToken(resp.Jwt)

	return nil
}

func main() {
	log.Println("getGPRCConn")
	conn, err := getGRPCConn()
	if err != nil {
		log.Fatal("error connecting to api", err)
	}
	log.Println("login")
	if err := login(conn); err != nil {
		log.Fatal("login error", err)
	}

	devClient := api.NewDeviceServiceClient(conn)
	dev := api.Device{
		Name:            "RM1-SS7",
		DevEui:          "007032FFFE010006",
		DeviceProfileId: "9e89a1f2-fbbf-46fa-840d-73f238053bbd",
		Description:     "Soil HydraSensor - Temperature/Volumetric Water Content/Electrical Conductivity/Salinty",
		ApplicationId:   2, // Soil-HydraSensor
		SkipFCntCheck:   true}
	devReq := api.CreateDeviceRequest{Device: &dev}
	_, err1 := devClient.Create(context.Background(), &devReq)
	if err1 != nil {
		log.Fatal("err create device", err)
	}

	devKey := api.DeviceKeys{
		DevEui:    "007032FFFE010006",
		NwkKey:    "a05b6d3d1c2446149e6e8949c94f231a",
		AppKey:    "415164cd36354b168d67fa251573969b",
		GenAppKey: "c733489f95c8a1353ec4000369bb522c",
	}

	_, err2 := devClient.CreateKeys(context.Background(),
		&api.CreateDeviceKeysRequest{
			DeviceKeys: &devKey})
	if err2 != nil {
		if grpc.Code(err) == codes.AlreadyExists {
			log.Printf("device-keys for device %s already exists", dev.DevEui)
		}
		log.Fatalf("import error (device %s): %s", dev.DevEui, err)
	}

}
